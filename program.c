#include "program.h"

#include <stdbool.h>

bool compile_shader(GLint * shader_id, const char *path)
{
	FILE *file = fopen(path, "r");
	fseek(file, 0L, SEEK_END);
	size_t length = ftell(file) + 1;

	char *source = calloc(sizeof(char), length);

	rewind(file);
	INFO("Read %zu bytes from shader.",
		 fread(source, sizeof(char), length, file));
	fclose(file);

	glShaderSource(*shader_id, 1, (const char **) &source, NULL);

	GLint compiled = GL_FALSE;

	glCompileShader(*shader_id);
	glGetShaderiv(*shader_id, GL_COMPILE_STATUS, &compiled);

	if (compiled == GL_FALSE) {
		char error_txt[1024];
		int max_length = 1024;
		glGetShaderInfoLog(*shader_id, 1024, &max_length, error_txt);
		FATAL("Couldn't compile shader '%s'.\n%s", path, error_txt);
		return false;
	}
	free(source);
	return true;
}

program_t program_new(const char *vs_path, const char *fs_path)
{
	program_t program;
	GLint vs_id, fs_id;

	vs_id = glCreateShader(GL_VERTEX_SHADER);
	compile_shader(&vs_id, vs_path);

	fs_id = glCreateShader(GL_FRAGMENT_SHADER);
	compile_shader(&fs_id, fs_path);

	/* And now, create the program. */
	program.id = glCreateProgram();

	glAttachShader(program.id, vs_id);
	glAttachShader(program.id, fs_id);

	// Binding attribute locations to the program.
	glBindAttribLocation(program.id, 0, "position");
	glBindAttribLocation(program.id, 1, "normal");
	glBindAttribLocation(program.id, 2, "uv");
	glUniform1i(glGetUniformLocation(program.id, "texture_1"), 0);

	glLinkProgram(program.id);
	GLint linked = GL_FALSE;
	glGetProgramiv(program.id, GL_LINK_STATUS, &linked);

	if (!linked) {
		char error_txt[1024];
		int max_length = 1024;
		glGetShaderInfoLog(fs_id, 1024, &max_length, error_txt);
		FATAL("Couldn't link program.\n%s", error_txt);
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
