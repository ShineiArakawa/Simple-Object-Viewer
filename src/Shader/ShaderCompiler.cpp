#include <Shader/ShaderCompiler.hpp>

namespace simview {
namespace shader {

std::string ShaderCompiler::readCodesFromFile(const std::string& filename) {
  // Load source file
  std::ifstream reader;
  std::string code;

  // Open source file
  reader.open(filename.c_str(), std::ios::in);
  if (!reader.is_open()) {
    // Finish with error message if source file could not be opened
    LOG_CRITICAL("Failed to load a shader: " + filename);
    exit(1);
  }

  // Load entire contents of a file and store to a string variable
  {
    // Move seek position to the end
    reader.seekg(0, std::ios::end);
    // Reserve memory location for code characters
    code.reserve(reader.tellg());
    // Move seek position back to the beginning
    reader.seekg(0, std::ios::beg);

    // Load entire file and copy to "code" variable
    code.assign(std::istreambuf_iterator<char>(reader), std::istreambuf_iterator<char>());
  }

  // Close file
  reader.close();

  return code;
}

GLuint ShaderCompiler::compile(const std::string& code, GLuint type) {
  LOG_DEBUG("[Code]");
  LOG_DEBUG(code);

  // Create a shader
  GLuint shaderId = glCreateShader(type);
  LOG_DEBUG("Done. glCreateShader");

  // Compile a source code
  const char* codeChars = code.c_str();
  glShaderSource(shaderId, 1, &codeChars, NULL);
  LOG_DEBUG("Done. glShaderSource");
  glCompileShader(shaderId);
  LOG_DEBUG("Done. glCompileShader");

  // Check whther compile is successful
  GLint compileStatus;
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
  if (compileStatus == GL_FALSE) {
    // Terminate with error message if compilation failed
    LOG_CRITICAL("Failed to compile a shader!");

    // Get length of error message
    GLint logLength;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      // Get error message
      GLsizei length;
      std::string errMsg;
      errMsg.resize(logLength);
      glGetShaderInfoLog(shaderId, logLength, &length, &errMsg[0]);

      // Print error message and corresponding source code
      LOG_CRITICAL(errMsg);
      LOG_CRITICAL(code);
    }
    exit(1);
  }

  return shaderId;
}

GLuint ShaderCompiler::buildShaderProgram(const std::string& vertexShaderCode,
                                          const std::string& fragmentShaderCode) {
  // Compile shader files
  GLuint vertShaderId = ShaderCompiler::compile(vertexShaderCode, GL_VERTEX_SHADER);
  LOG_DEBUG("Compiling vertex shader done.");
  GLuint fragShaderId = ShaderCompiler::compile(fragmentShaderCode, GL_FRAGMENT_SHADER);
  LOG_DEBUG("Compiling fragment shader done.");

  // Link shader objects to the program
  GLuint programId = glCreateProgram();
  glAttachShader(programId, vertShaderId);
  glAttachShader(programId, fragShaderId);
  glLinkProgram(programId);

  // Check whether link is successful
  GLint linkState;
  glGetProgramiv(programId, GL_LINK_STATUS, &linkState);
  if (linkState == GL_FALSE) {
    // Terminate with error message if link is failed
    LOG_CRITICAL("Failed to link shaders!");

    // Get length of error message
    GLint logLength;
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      // Get error message
      GLsizei length;
      std::string errMsg;
      errMsg.resize(logLength);
      glGetProgramInfoLog(programId, logLength, &length, &errMsg[0]);

      // Print error message
      LOG_CRITICAL(errMsg);
    }
    exit(1);
  }

  // Disable shader program and return its ID
  glUseProgram(0);

  return programId;
}

}  // namespace shader
}  // namespace simview