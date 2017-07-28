package main

import (
	"os"
	"text/template"

	"github.com/Sirupsen/logrus"
	"github.com/urfave/cli"

	"github.com/0x0916/notes/generate"
)

var VERSION = "v0.0.0-dev"

const (
	SITEDIR = "./public"
)

func renderIndex(examples []*generate.CodeExample, lang string) {
	logrus.Debug("Render Index....")
	indexTmpl := template.New("index")
	_, err := indexTmpl.Parse(mustReadFile("templates/" + lang + "/index.tmpl"))
	if err != nil {
		logrus.Fatal(err)
	}

	if err := os.MkdirAll(SITEDIR+"/"+lang, 0755); err != nil {
		logrus.Fatal(err)
	}

	indexF, err := os.Create(SITEDIR + "/" + lang + "/index.html")
	if err != nil {
		logrus.Fatal(err)
	}
	indexTmpl.Execute(indexF, examples)
}

func renderCodeExamples(examples []*generate.CodeExample, lang string) {
	logrus.Debug("Render Code Example....")
	exampleTmpl := template.New("code-example")
	_, err := exampleTmpl.Parse(mustReadFile("templates/" + lang + "/code-example.tmpl"))
	if err != nil {
		logrus.Fatal(err)
	}
	if err := os.MkdirAll(SITEDIR+"/"+lang, 0755); err != nil {
		logrus.Fatal(err)
	}

	for _, example := range examples {
		exampleF, err := os.Create(SITEDIR + "/" + lang + "/" + example.ID + ".html")
		if err != nil {
			logrus.Fatal(err)
		}
		exampleTmpl.Execute(exampleF, example)
	}
}

func generateWebsite(index, lang string) {
	setupStaticFiles(lang)
	codeExamples := generate.ParseCodeExamples(index, lang)
	renderIndex(codeExamples, lang)
	renderCodeExamples(codeExamples, lang)

}

func main() {
	app := cli.NewApp()
	app.Name = "notes"
	app.Version = VERSION
	app.Usage = "You need help!"
	app.Flags = []cli.Flag{
		cli.BoolFlag{
			Name:  "debug, D",
			Usage: "enable debug output for logging",
		},
	}
	app.Before = func(context *cli.Context) error {
		if context.GlobalBool("debug") {
			logrus.SetLevel(logrus.DebugLevel)
		}
		return nil
	}
	app.Action = func(context *cli.Context) error {
		generateWebsite("go.index.md", "go")
		generateWebsite("c.index.md", "c")
		generateWebsite("blog.index.md", "blog")
		return nil
	}

	if err := app.Run(os.Args); err != nil {
		logrus.Fatal(err)
	}
}
