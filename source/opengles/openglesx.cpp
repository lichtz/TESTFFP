#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include <android/native_window.h>
#include <unistd.h>
#include "android/log.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "EsUtil.h"


#define TAG "zj"

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
typedef struct {
    // Handle to a program object
    GLuint programObject;

} UserData;

GLuint LoadShader(GLenum type, const char *shaderSrc) {
    GLuint shader;
    GLint compiled;

    // Create the shader object
    shader = glCreateShader(type);

    if (shader == 0) {
        return 0;
    }

    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, NULL);

    // Compile the shader
    glCompileShader(shader);

    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLint infoLen = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            char *infoLog = static_cast<char *>(malloc(sizeof(char) * infoLen));
            glGetShaderInfoLog(shader, infoLen, nullptr, infoLog);
            LOGE("Error compiling shader:\n%s\n", infoLog);

            free(infoLog);
        }

        glDeleteShader(shader);
        return 0;
    }

    return shader;

}

int Init(ESContent *esContext) {
    UserData *userData = static_cast<UserData *>(esContext->userData);
    char vShaderStr[] =
            "#version 300 es                          \n"
            "layout(location = 0) in vec4 vPosition;  \n"
            "layout (location = 1) in vec2 aTexCoords; \n"
            "out vec2 textureCoordinate;         \n"
            "void main()                              \n"
            "{                                        \n"
            " textureCoordinate = aTexCoords;         \n"
            "   gl_Position = vPosition;              \n"
            "}                                        \n";

    char fShaderStr[] =
            "#version 300 es                              \n"
            "#extension GL_OES_EGL_image_external_essl3 : require \n"
            "precision mediump float;                      \n"
            "uniform samplerExternalOES vTexture;          \n"
            "in vec2 textureCoordinate;         \n"
            "out vec4 vFragColor;         \n"
            "void main()                                  \n"
            "{                                            \n"
            "   vFragColor  = texture (vTexture, textureCoordinate  );  \n"
            "}                                            \n";

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;

    // Load the vertex/fragment shaders
    vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

    // Create the program object
    programObject = glCreateProgram();

    if (programObject == 0) {
        return 0;
    }

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    // Link the program
    glLinkProgram(programObject);

    // Check the link status
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

    if (!linked) {
        GLint infoLen = 0;

        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            char *infoLog = static_cast<char *>(malloc(sizeof(char) * infoLen));

            glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
            LOGE("Error linking program:\n%s\n", infoLog);

            free(infoLog);
        }

        glDeleteProgram(programObject);
        return GL_FALSE;
    }

    // Store the program object
    userData->programObject = programObject;

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    return GL_TRUE;
}

GLfloat vVertices[] = {
        1.0f, 1.0f, 0.0f,       1.0f,1.0f,// 右上角
        1.0f, -1.0f, 0.0f,      1.0f,0.0f, // 右下角
        -1.0f, -1.0f, 0.0f,     0.0f,0.0f,// 左下角
        -1.0f, 1.0f, 0.0f ,      0.0f,1.0f,// 左上角

};
unsigned int indices[] = { // 注意索引从0开始!
        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
};


void genTextures(ESContent *esContent) {
    glGenTextures(1, &esContent->textId);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, esContent->textId);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, esContent->textId);
    GLint vIndex=glGetUniformLocation(static_cast<UserData *>(esContent->userData)->programObject,"vTexture");
    glUniform1f(vIndex,0);
    LOGE(">>>>>>vTexture%d   ",vIndex);

}

void genBO(ESContent *esContent) {
    GLint vCoorIndex=glGetAttribLocation(static_cast<UserData *>(esContent->userData)->programObject,"aTexCoords");
    LOGE(">>>>>>vCoorIndex%d   ",vCoorIndex);
    glGenBuffers(1, &esContent->VBO);
    glGenBuffers(1, &esContent->EBO);
    glGenBuffers(1, &esContent->VAO);
    glGenBuffers(1,&esContent->TEXTURES);
    glBindVertexArray(esContent->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, esContent->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, esContent->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3*sizeof (float )));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


}

ESContent *esContent;
bool destroyRequested = false;
float lastTime;
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_opengles_EGLRender_initEGL(JNIEnv *env, jobject thiz, jobject surface) {
    esContent = new ESContent();
    esContent->userData = malloc(sizeof(UserData));
    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
    esContent->eglNativeWindowType = window;
    esContent->width = ANativeWindow_getHeight(window);
    esContent->height = ANativeWindow_getWidth(window);
    esContent->eglNativeDisplayType = EGL_DEFAULT_DISPLAY;
    esCreateWindow(esContent, esContent->width, esContent->height);
    if (!Init(esContent)) {
        LOGE("GL INIT ERROR");
    }
    genTextures(esContent);
    genBO(esContent);
    destroyRequested = false;
    LOGE("textID%d", esContent->textId);
    return esContent->textId;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_opengles_EGLRender_setViewPort(JNIEnv *env, jobject thiz, jint view_port, jint h) {
    glViewport(0, 0, view_port, h);

}

static float GetCurrentTime() {
    struct timespec clockRealTime;
    clock_gettime(CLOCK_MONOTONIC, &clockRealTime);
    double curTimeInSeconds = clockRealTime.tv_sec + (double) clockRealTime.tv_nsec / 1e9;
    return (float) curTimeInSeconds;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_opengles_EGLRender_egl_1DRAW(JNIEnv *env, jobject thiz) {
    lastTime = GetCurrentTime();
//    while (true) {
        if (destroyRequested) {
            return;
        }
        {
            float curTime = GetCurrentTime();
            float deltaTime = (curTime - lastTime);
            lastTime = curTime;
            LOGE("DRAW pps >%f",deltaTime);

        }
        UserData *userData = static_cast<UserData *>(esContent->userData);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(userData->programObject);
        glBindVertexArray(esContent->VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        eglSwapBuffers(esContent->eglDisplay, esContent->eglSurface);
//    }

}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_opengles_EGLRender_egl_1Destory(JNIEnv *env, jobject thiz) {
    destroyRequested = true;
    glDeleteVertexArrays(1, &esContent->VAO);
    glDeleteBuffers(1, &esContent->VBO);
    glDeleteBuffers(1, &esContent->EBO);
    glDeleteProgram(static_cast<UserData *>(esContent->userData)->programObject);
}