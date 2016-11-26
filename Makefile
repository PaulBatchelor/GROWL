growl.so: growl.c
	$(CC) -fPIC -shared $< -o $@ -lsporth -lsoundpipe

clean:
	rm -rf growl.so
