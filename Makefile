growl.so: growl.c
	$(CC) -DBUILD_PLUGIN -fPIC -shared $< -o $@ -lsporth -lsoundpipe

clean:
	rm -rf growl.so
