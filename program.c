#include "program.h"

program_t program_new(const char *vs_path, const char *fs_path)
{
	program_t program;
	GLint vs_id, fs_id;

	/* First, load the vertex shader. */

	vs_id = glCreateShader(GL_VERTEX_SHADER);

	FILE *file = fopen(vs_path, "r");
	fseek(file, 0L, SEEK_END);

	size_t size = ftell(file);
	char *source = calloc(1, size);

	rewind(file);
	int _idontfuckingcare = fread(source, size, sizeof(char), file);
	(void) _idontfuckingcare;
	fclose(file);

	glShaderSource(vs_id, 1, (const char *const *) &source, NULL);

	GLint compiled = GL_FALSE;

	glCompileShader(vs_id);
	glGetShaderiv(vs_id, GL_COMPILE_STATUS, &compiled);

	if (compiled == GL_FALSE) {
		char error_txt[1024];
		int max_length = 1024;
		glGetShaderInfoLog(vs_id, 1024, &max_length, error_txt);
		FATAL("Couldn't compile vertex shader.\n%s", error_txt);
	}

	/* Then, load the fragment shader. */

	fs_id = glCreateShader(GL_FRAGMENT_SHADER);

	file = fopen(fs_path, "r");
	fseek(file, 0L, SEEK_END);

	size = ftell(file);
	source = calloc(1, size);

	rewind(file);
	_idontfuckingcare = fread(source, size, sizeof(char), file);
	fclose(file);

	glShaderSource(fs_id, 1, (const char *const *) &source, NULL);

	compiled = GL_FALSE;

	glCompileShader(fs_id);
	glGetShaderiv(fs_id, GL_COMPILE_STATUS, &compiled);

	if (compiled == GL_FALSE) {
		char error_txt[1024];
		int max_length = 1024;
		glGetShaderInfoLog(fs_id, 1024, &max_length, error_txt);
		FATAL("Couldn't compile fragment shader.\n%s", error_txt);
	}

	/* And now, create the program. */
	program.id = glCreateProgram();

	glAttachShader(program.id, vs_id);
	glAttachShader(program.id, fs_id);

	// Binding attribute locations to the program.
	glBindAttribLocation(program.id, 0, "position");

	glLinkProgram(program.id);
	GLint linked = GL_FALSE;
	glGetProgramiv(program.id, GL_LINK_STATUS, &linked);

	if (!linked) {
		FATAL("Couldn't link program.");
	}

	return program;
}

void program_use(program_t * program)
{
	glUseProgram(program->id);
}

void program_terminate(program_t * program)
{
	glDeleteProgram(program->id);
}
