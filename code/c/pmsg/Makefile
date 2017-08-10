all:
	gcc pmsg_unlink.c -lrt -o pmsg_unlink
	gcc pmsg_create.c -lrt -o pmsg_create
	gcc pmsg_getattr.c -lrt -o pmsg_getattr
	gcc pmsg_send.c -lrt -o pmsg_send
	gcc pmsg_receive.c -lrt -o pmsg_receive
clean:
	$(RM) pmsg_unlink pmsg_create pmsg_getattr pmsg_send pmsg_receive
