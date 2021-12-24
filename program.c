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

	glLinkProgram(program.id);
	GLint linked = GL_FALSE;
	glGetProgramiv(program.id, GL_LINK_STATUS, &linked);

	if (!linked) {
		char error_txt[1024];
		int max_length = 1024;
		glGetShaderInfoLog(fs_id, 1024, &max_length, error_txt);
		FATAL("Couldn't link program.\n%s", error_txt);
	}
	INFO("Done loading program %d", program.id);

	DEBUG_GL();
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

void program_debug(program_t * program)
{
	GLint current_program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
	if (current_program != program->id) {
		INFO("Current program (%d) is not debugged program (%d)", current_program, program->id);
	} else {
		INFO("Current program(%d) is active.", current_program);
	}
	GLint uniforms_no, attributes_no;
	GLint size;
	GLenum type;
	const GLsizei bufSize = 16;
	GLchar name[bufSize];
	GLsizei length;
	glGetProgramiv(program->id, GL_ACTIVE_UNIFORMS, &uniforms_no);
	glGetProgramiv(program->id, GL_ACTIVE_ATTRIBUTES, &attributes_no);
	INFO("Uniforms in program: %d", uniforms_no);
	INFO("Attributes in program: %d", attributes_no);
	for(GLint i = 0; i < uniforms_no; i++) {
		glGetActiveUniform(uniforms_no, i, bufSize, &length, &size, &type, name);
		INFO("Uniform #%d Type: %u Name: %s\n", i, type, name);
	}

	for (GLint i = 0; i < attributes_no; i++)
	{
		glGetActiveAttrib(program->id, (GLuint)i, bufSize, &length, &size, &type, name);

		INFO("Attribute #%d Type: %u Name: %s\n", i, type, name);
	}
}