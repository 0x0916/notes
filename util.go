package main

import (
	"io/ioutil"
	"os"

	"github.com/Sirupsen/logrus"
)

func copyFile(src, dst string) {
	dat, err := ioutil.ReadFile(src)
	if err != nil {
		logrus.Fatal(err)
	}
	err = ioutil.WriteFile(dst, dat, 0644)
	if err != nil {
		logrus.Fatal(err)
	}
}

func setupStaticFiles(lang string) {
	err := os.RemoveAll(SITEDIR + "/" + lang)
	if err != nil {
		logrus.Fatal(err)
	}

	err = os.MkdirAll(SITEDIR+"/"+lang, 0755)
	if err != nil {
		logrus.Fatal(err)
	}
	copyFile("templates/site.css", SITEDIR+"/"+lang+"/site.css")
}

func mustReadFile(path string) string {
	bytes, err := ioutil.ReadFile(path)
	if err != nil {
		logrus.Fatal(err)
	}
	return string(bytes)
}
