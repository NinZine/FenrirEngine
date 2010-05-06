#import <UIKit/UIKit.h>

const char*
filename_to_path(const char *filename)
{
	NSString *path;
	char *name, *type;
	
	name = strdup(filename);
	type = strrchr(name, '.');
	if (NULL == type) return type;
	
	*type = '\0';
	type = type	+ 1;
	
	/* TODO: Parse directory. */
	
	path = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:name]
										   ofType:[NSString stringWithUTF8String:type]];
	
	free(name);
	return [path cStringUsingEncoding:1];
}

const char*
full_path()
{
	NSString *p;
	
	p = [[NSBundle mainBundle] bundlePath];
	return [p cStringUsingEncoding:1];
}

char*
full_path_to_file(const char *filename)
{
	const char *p;
	char *f;
	
	p = full_path();
	f = malloc((strlen(p) + strlen(filename) + 2) * sizeof(char));
	strcpy(f, p);
	strcat(f, "/");
	strcat(f, filename);
	
	return f;
}

FILE*
ifopen(const char *name, const char *mode)
{
	const char *fullpath;
	
	fullpath = full_path_to_file(name);
	
	return fopen(fullpath, mode);
}
