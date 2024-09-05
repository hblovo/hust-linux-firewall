package core

import "web_backend/initialize"

type server interface {
	ListenAndServe() error
}

func RunWindowsServer() {
	Router := initialize.Routers()
	address := "127.0.0.1:8888"
	s := initServer(address, Router)
	s.ListenAndServe()
}
