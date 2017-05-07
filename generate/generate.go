package generate

import (
	"bytes"
	"io"
	"io/ioutil"
	"os"
	"os/exec"
	"path/filepath"
	"regexp"
	"strings"

	"github.com/Sirupsen/logrus"
	"github.com/russross/blackfriday"
)

type Language struct {
	name           string
	symbol         string
	commentMatcher *regexp.Regexp
	dividerText    string
	dividerHTML    *regexp.Regexp
}

var languages map[string]*Language

func setupLanguages() {
	languages = make(map[string]*Language)

	languages[".go"] = &Language{"go", "//", nil, "", nil}
	languages[".c"] = &Language{"c", "//", nil, "", nil}
	languages[".h"] = &Language{"c", "//", nil, "", nil}
	languages[".sh"] = &Language{"bash", "#", nil, "", nil}
}

func init() {
	setupLanguages()

	for _, lang := range languages {
		lang.commentMatcher, _ = regexp.Compile("^\\s*" + lang.symbol + "\\s?")
		lang.dividerText = "\n" + lang.symbol + "DIVIDER\n"
		lang.dividerHTML, _ = regexp.Compile("\\n*<span class=\"c1?\">" + lang.symbol + "DIVIDER<\\/span>\\n*")
	}
}

// Wrap the code in these
const highlightStart = "<div class=\"highlight\"><pre>"
const highlightEnd = "</pre></div>"

func getLanguage(source string) *Language {
	return languages[filepath.Ext(source)]
}

func ensureDir(dir string) {
	err := os.MkdirAll(dir, 0755)
	if err != nil {
		logrus.Fatal(err)
	}
}

func mustReadFile(path string) string {
	bytes, err := ioutil.ReadFile(path)
	if err != nil {
		logrus.Fatal(err)
	}
	return string(bytes)
}

func markdownSummary(path string) string {
	summary, err := ioutil.ReadFile(path)
	if err != nil {
		logrus.Fatal(err)
	}

	return string(blackfriday.MarkdownCommon(summary))
}

func readLines(path string) []string {
	src := mustReadFile(path)
	return strings.Split(src, "\n")
}

func mustGlob(glob string) []string {
	paths, err := filepath.Glob(glob)
	if err != nil {
		logrus.Fatal(err)
	}
	return paths
}

var dashPat = regexp.MustCompile("\\-+")

type Section struct {
	docsText string
	codeText string
	DocsHTML string
	CodeHTML string
}

type CodeExample struct {
	ID          string
	Name        string
	SummaryHTML string
	Sections    [][]*Section
	Next        *CodeExample
	Prev        *CodeExample
}

func parseSections(source string, code []byte) []*Section {
	lines := bytes.Split(code, []byte("\n"))
	sections := []*Section{}
	language := getLanguage(source)

	var hasCode bool
	var codeText = new(bytes.Buffer)
	var docsText = new(bytes.Buffer)

	// save a new section
	save := func(docs, code string) {
		newSection := Section{docsText: docs, codeText: code}
		sections = append(sections, &newSection)
	}

	for _, line := range lines {
		// if the line is a comment
		if language.commentMatcher.Match(line) {
			// but there was previous code
			if hasCode {
				// we need to save the existing documentation and text
				// as a section and start a new section since code blocks
				// have to be delimited before being sent to Pygments
				save(docsText.String(), codeText.String())
				hasCode = false
				codeText.Reset()
				docsText.Reset()
			}
			docsText.Write(language.commentMatcher.ReplaceAll(line, nil))
			docsText.WriteString("\n")
		} else {
			hasCode = true
			codeText.Write(line)
			codeText.WriteString("\n")
		}
	}
	// save any remaining parts of the source file
	save(docsText.String(), codeText.String())
	return sections
}

// `highlight` pipes the source to Pygments, section by section
// delimited by dividerText, then reads back the highlighted output,
// searches for the delimiters and extracts the HTML version of the code
// and documentation for each `Section`
func highlight(source string, sections []*Section) {
	language := getLanguage(source)
	pygments := exec.Command("pygmentize", "-l", language.name, "-f", "html", "-O", "encoding=utf-8")
	pygmentsInput, _ := pygments.StdinPipe()
	pygmentsOutput, _ := pygments.StdoutPipe()
	// start the process before we start piping data to it
	// otherwise the pipe may block
	pygments.Start()
	for i, s := range sections {
		pygmentsInput.Write([]byte(s.codeText))
		if i != len(sections)-1 {
			io.WriteString(pygmentsInput, language.dividerText)
		}
	}
	pygmentsInput.Close()

	buf := new(bytes.Buffer)
	io.Copy(buf, pygmentsOutput)

	output := buf.Bytes()
	output = bytes.Replace(output, []byte(highlightStart), nil, -1)
	output = bytes.Replace(output, []byte(highlightEnd), nil, -1)

	for _, s := range sections {
		index := language.dividerHTML.FindIndex(output)
		if index == nil {
			index = []int{len(output), len(output)}
		}

		fragment := output[0:index[0]]
		output = output[index[1]:]
		s.CodeHTML = string(bytes.Join([][]byte{[]byte(highlightStart), []byte(highlightEnd)}, fragment))
		s.DocsHTML = string(blackfriday.MarkdownCommon([]byte(s.docsText)))
	}
}

func parseAndRenderSections(sourcePath string) []*Section {
	code, err := ioutil.ReadFile(sourcePath)
	if err != nil {
		logrus.Fatal(err)
	}

	sections := parseSections(sourcePath, code)
	highlight(sourcePath, sections)

	return sections
}

func cleanSourcePaths(raw []string) []string {
	var result []string
	var files []string
	var ignorePattern []string
	var ignoreFile string

	for _, file := range raw {
		if strings.HasSuffix(file, "ignore") {
			ignoreFile = file
		} else {
			files = append(files, file)
		}
	}

	if ignoreFile == "" {
		ignorePattern = append(ignorePattern, "a.out")
	} else {
		ignorePattern = readLines(ignoreFile)
	}

	var skip bool
	for _, file := range files {
		skip = false
		for _, pat := range ignorePattern {
			if pat == "" {
				continue
			}
			if strings.HasSuffix(file, pat) {
				logrus.Debugf("skip file %s ...", file)
				skip = true
				break
			}
		}
		if !skip {
			result = append(result, file)
		}
	}

	return result
}

func ParseCodeExamples(index, lang string) []*CodeExample {
	exampleNames := readLines(index)
	examples := make([]*CodeExample, 0)
	for _, exampleName := range exampleNames {
		if (exampleName != "") && !strings.HasPrefix(exampleName, "#") {
			exampleNameZh := exampleName
			if strings.Index(exampleName, "->") != -1 {
				names := strings.Split(exampleName, "->")
				exampleName = names[0]
				if strings.Trim(names[1], " ") != "" {
					exampleNameZh = names[1]
				} else {
					exampleNameZh = names[0]
				}
			}
			example := CodeExample{Name: exampleNameZh}
			exampleID := strings.ToLower(exampleName)
			exampleID = strings.Replace(exampleID, " ", "-", -1)
			exampleID = strings.Replace(exampleID, "/", "-", -1)
			exampleID = strings.Replace(exampleID, "'", "", -1)
			exampleID = dashPat.ReplaceAllString(exampleID, "-")
			example.ID = exampleID
			example.Sections = make([][]*Section, 0)
			rawSourcePaths := mustGlob("code/" + lang + "/" + exampleID + "/*")
			sourcePaths := cleanSourcePaths(rawSourcePaths)
			for _, sourcePath := range sourcePaths {
				logrus.Debugf("process file %s ...", sourcePath)
				if strings.HasSuffix(sourcePath, ".README.MD") ||
					strings.HasSuffix(sourcePath, ".md") {
					example.SummaryHTML = markdownSummary(sourcePath)
				} else {
					sourceSection := parseAndRenderSections(sourcePath)
					example.Sections = append(example.Sections, sourceSection)
				}
			}
			examples = append(examples, &example)
		}
	}
	for i, example := range examples {
		if i < (len(examples) - 1) {
			example.Next = examples[i+1]
		}
		if i > 0 {
			example.Prev = examples[i-1]
		}
	}
	return examples
}
