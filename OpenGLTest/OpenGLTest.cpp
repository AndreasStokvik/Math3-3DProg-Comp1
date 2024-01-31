#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iomanip>
#include <functional>
#include <glm/glm.hpp>



// Vertex Shader
const char* vertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec3 aPosition;\n"
"uniform float slopeValue;\n"
"void main() {\n"
"    gl_Position = vec4(aPosition, 1.0);\n"
"}\0";

// Fragment Shader
const char* fragmentShaderSource =
"#version 330 core\n"
"uniform float slopeValue;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"   if (slopeValue <= 0) {\n"
"       FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n" // Red for non-positive slope
"   } else {\n"
"       FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n" // Green for positive slope
"   }\n"
"}\0";



// --- 2D TEXT OUTPUT ---
void writeVertexDataToFile(const char* filenameVertex, const char* filenameData, const char* filenameRaw, int numPoints, GLfloat* vertices, float* originalYValues, float* originalZValues, float xMin, float xMax) {
    std::ofstream outputFileVertex(filenameVertex);
    std::ofstream outputFileRaw(filenameRaw);
    std::ofstream outputFileData(filenameData);

    // Check if files are opened successfully
    if (!outputFileVertex.is_open() || !outputFileData.is_open() || !outputFileRaw.is_open()) {
        std::cerr << "Error: Unable to open files for writing." << std::endl;
        return;
    }

    // Write the number of points as the first line
    outputFileVertex << "Number of Points: " << numPoints << std::endl;
    outputFileRaw << "Number of Points: " << numPoints << std::endl;
    outputFileData << "Number of Points: " << numPoints << std::endl;

    float xInc = (xMax - xMin) / (numPoints - 1);

    outputFileVertex << std::fixed << std::setprecision(6);
    outputFileData << std::fixed << std::setprecision(6);

    // Write each subsequent line with vertex data and original y-values
    for (int i = 0; i < numPoints; i++) {
        float x1 = vertices[i * 3];
        float y1 = vertices[i * 3 + 1];
        float z1 = vertices[i * 3 + 2];
        float x2 = vertices[(i + 1) * 3];
        float y2 = vertices[(i + 1) * 3 + 1];
        float z2 = vertices[(i + 1) * 3 + 2];

        // Calculate Newtonian quotient (slope)
        float slopeValue = (y2 - y1) / (x2 - x1);

        outputFileVertex << i + 1 << ":\t x: " << x1 << "\ty: " << y1 << "\tz: " << z1 << std::endl;
        outputFileRaw << x1 << ", " << y1 << ", " << z1 << std::endl;
        outputFileData << "x: " << xMin + (i * xInc) << "\ty: " << originalYValues[i] << "\tSlope: " << slopeValue << std::endl;
    }

    outputFileVertex.close();
    outputFileData.close();
}



// --- 3D TEXT OUTPUT ---
void writeVertexDataToFile3D(const char* filenameVertex, const char* filenameRaw, int numPoints, GLfloat* vertices, float* originalYValues, float* originalZValues, float xMin, float xMax) {
    std::ofstream outputFileVertex(filenameVertex);
    std::ofstream outputFileRaw(filenameRaw);

    // Write the number of points as the first line
    outputFileVertex << "Number of Points: " << numPoints * numPoints << std::endl;
    outputFileRaw << "Number of Points: " << numPoints*numPoints << std::endl;

    outputFileVertex << std::fixed << std::setprecision(6);

    // Write each subsequent line with vertex data and original y-values
    for (int i = 0; i < numPoints * numPoints; i++) {
        float x1 = vertices[i * 3];
        float y1 = vertices[i * 3 + 1];
        float z1 = vertices[i * 3 + 2];
        float x2 = vertices[(i + 1) * 3];
        float y2 = vertices[(i + 1) * 3 + 1];
        float z2 = vertices[(i + 1) * 3 + 2];


        outputFileVertex << i+1 << ":\t x: " << x1 << "\ty: " << y1 << "\tz: " << z1 << std::endl;
        outputFileRaw << x1 << ", " << y1 << ", " << z1 << std::endl;
    }

    outputFileVertex.close();
}



// --- 2D LINE DRAW METHOD ---
// Works for 2D graphs and the spiral
void drawGraphLines2D(GLuint VAO, int numPoints, GLuint shaderProgram, GLint slopeValueLocation, GLfloat* vertices) {
    // Use the shader program
    glUseProgram(shaderProgram);

    // Draw each segment separately
    for (int i = 0; i < numPoints-1; ++i) {
        float x1 = vertices[i * 3];
        float y1 = vertices[i * 3 + 1];
        float z1 = vertices[i * 3 + 2];
        float x2 = vertices[(i + 1) * 3];
        float y2 = vertices[(i + 1) * 3 + 1];
        float z2 = vertices[(i + 1) * 3 + 2];

        // Calculate Newtonian quotient (slope)
        float slopeValue = (y2 - y1) / (x2 - x1);

        // Set the uniform slope value for the current segment
        glUniform1f(slopeValueLocation, slopeValue);

        // Draw the current segment
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_STRIP, i, 2); // Draw two vertices to form a line segment
        glBindVertexArray(0);
    }
}


// --- DRAW LINES IN 3D SPACE ---
// Works for 3d graphs (functions of 2 variables)
void drawGraphLines3D(GLuint VAO, int numPoints, GLuint shaderProgram, GLint slopeValueLocation, GLfloat* vertices) {
    // Use the shader program
    glUseProgram(shaderProgram);

    // Draw each segment separately
    for (int i = 0; i < numPoints - 2; ++i) {
        float x1 = vertices[i * 3];
        float y1 = vertices[i * 3 + 1];
        float z1 = vertices[i * 3 + 2];
        float x2 = vertices[(i + 1) * 3];
        float y2 = vertices[(i + 1) * 3 + 1];
        float z2 = vertices[(i + 1) * 3 + 2];

        // Calculate Newtonian quotient (slope)
        float slopeValue = (y2 - y1) / (x2 - x1);

        // Set the uniform slope value for the current segment
        glUniform1f(slopeValueLocation, slopeValue);

        // Draw the current segment
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_STRIP, i, 2); // Draw two vertices to form a line segment
        glBindVertexArray(0);
    }
}



// --- 2D GRAPH ---
// Calculate a graph in 2D-space (z coordinate is just set to 0)
void calculateGraph2D(GLfloat* vertices, float xMin, float xMax, int numPoints, std::function<float(float)> expression) {

    float xRange = xMax - xMin;
    float xOffset = (xMax + xMin) / 2.0f;

    float yMin = std::numeric_limits<float>::infinity();
    float yMax = -std::numeric_limits<float>::infinity();

    // Create a vector to store original y-values
    std::vector<float> originalYValues(numPoints);

    // Calculate yRange and find yMin and yMax
    for (int i = 0; i < numPoints; i++) {
        float x = xMin + static_cast<float>(i) / static_cast<float>(numPoints - 1) * xRange;
        float y = expression(x);  // Use the provided expression

        originalYValues[i] = y;  // Store original y-values

        if (y < yMin) yMin = y;
        if (y > yMax) yMax = y;
    }

    float yRange = yMax - yMin;
    

    // Calculate vertices
    for (int i = 0; i < numPoints; i++) {
        float x = xMin + static_cast<float>(i) / static_cast<float>(numPoints - 1) * xRange;
        float y = expression(x);        // Uses the provided expression
        float z = 0.0f;                 // Left as 0 since graph is 2D

        vertices[i * 3] = (x - xOffset) / (xRange / 2);             // Normalize x within (-1, 1)
        vertices[i * 3 + 1] = 2.0f * ((y - yMin) / yRange) - 1.0f;  // Normalize y within (-1, 1)
        vertices[i * 3 + 2] = z;
    }

    // Call writeVertexDataToFile
    writeVertexDataToFile("vertex_data.txt", "function_data.txt", "vertex_raw.txt", numPoints, vertices, originalYValues.data(), 0, xMin, xMax);
}


// --- 3D SPIRAL ---
// Calculate vertex coords for a 3d spiral
void calculateSpiral(GLfloat* vertices, int numPoints) {
    float rad = 16.0f * atan(1);    // radius
    float dt = rad / (numPoints/2);


    // Create a vector to store original y-values
    std::vector<float> originalYValues(numPoints);
    std::vector<float> originalZValues(numPoints);


    // Calculate vertices for a 3D spiral
    for (int i = 0; i < numPoints; ++i) {
        float t = i * dt;

        float x = std::cos(t);
        float y = std::sin(t);
        float z = t/rad - 1;

        originalYValues[i] = y;
        originalZValues[i] = z;

        // Assign coordinates to vertices
        vertices[i * 3] = x;
        vertices[i * 3 + 1] = y;
        vertices[i * 3 + 2] = z;
    }

    // Call writeVertexDataToFile
    writeVertexDataToFile("vertex_data.txt", "function_data.txt", "vertex_raw.txt", numPoints, vertices, originalYValues.data(), originalZValues.data(), -1, 1);
}



// --- 3D GRAPH ---
// Calculate a graph in 3D-space
void calculateGraph3D(GLfloat* vertices, int numPoints, std::function<float(float, float)> expression) {
    float xMin = -1.0f;
    float xMax = 1.0f;
    float yMin = -1.0f;
    float yMax = 1.0f;

    // Calculate vertices
    int index = 0;
    for (int i = 0; i < numPoints; ++i) {
        for (int j = 0; j < numPoints; ++j) {
            float x = xMin + static_cast<float>(i) / static_cast<float>(numPoints - 1) * (xMax - xMin);
            float y = yMin + static_cast<float>(j) / static_cast<float>(numPoints - 1) * (yMax - yMin);
            float z = expression(x,y);  // Use the provided expression

            // Add x, y, z coordinates to vertices
            vertices[index * 3] = x;
            vertices[index * 3 + 1] = y;
            vertices[index * 3 + 2] = z;

            ++index;
        }
    }

    // Call writeVertexDataToFile
    writeVertexDataToFile3D("vertex_data.txt", "vertex_raw.txt", numPoints, vertices, 0, 0, -1, 1);

}



int main()
{
    // Initializing GLFW
    glfwInit();

    // Tell GLFW OpenGl Version (OpenGL 3.3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Tell GLFW OpenGL Profile (CORE)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFW window with dimensions 800x800, window title, fullscreen y/n, leave last option NULL because "its not important"
    GLFWwindow* window = glfwCreateWindow(800, 800, "Title", NULL, NULL);

    // Terminate GLFW if last code fails
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Tell GLFW to use the window
    glfwMakeContextCurrent(window);

    // Load GLAD so it configures OpenGL
    gladLoadGL();

    // Specify viewport dimensions of OpenGL
    glViewport(0, 0, 800, 800);



    // Graph values
    const int numPoints = 100;
    GLfloat vertices[3*(numPoints * numPoints)];
    float xMin = -10.0f;
    float xMax = 10.0f;

    // 2D-space function
    auto expression2d = [](float x) { return std::tan(x*x); };
    
    // 3D-space function
    auto expression3d = [](float x, float y) { return x * y; };


 // ----- WHEN SWITCHING BETWEEN 2D GRAPH/SPIRAL AND 3D GRAPH, REMEMBER TO SWITCH DRAW METHOD IN MAIN WHILE LOOP BELOW -----
    // Calculate vertex coords for 2D space graph
     calculateGraph2D(vertices, xMin, xMax, numPoints, expression2d);
    
    // Calculate vertex coords for 3D spiral
    // calculateSpiral(vertices, numPoints);

    // Calculate vertex coords for 3D space graph
    // calculateGraph3D(vertices, numPoints, expression3d);
// -------------------------------------------------------------------------------------------------------------------------


    // Create Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind VAO
    glBindVertexArray(VAO);

    // Bind VBO and copy data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Unbind VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);



    // Create and compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);



    // Create and compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);


    // Create shader program, attach shaders, and link
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // checks for errors (courtesy of ChatGPT)
    if (true) {
        // Check vertex shader compilation
        GLint success;
        GLchar infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "Vertex shader compilation failed:\n" << infoLog << std::endl;
        }

        // Check fragment shader compilation
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "Fragment shader compilation failed:\n" << infoLog << std::endl;
        }

        // Check shader program linking
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "Shader program linking failed:\n" << infoLog << std::endl;
        }
         
        // Check the location of the slope variable
        GLint slopeLocation = glGetUniformLocation(shaderProgram, "slopeValue");
        if (slopeLocation == -1) {
            std::cout << "Failed to get the location of the slope variable." << std::endl;
        }
         
        // Check if the vertex data is being passed correctly
        for (int i = 0; i < numPoints; i++) {
            // std::cout << "Vertex " << i << ": (" << vertices[i * 2] << ", " << vertices[i * 2 + 1] << ")\n";
            if (vertices[i * 2] > 1 or vertices[i * 2] < -1 or vertices[i * 2 + 1] > 1 or vertices[i * 2 + 1] < -1) {
                std::cout << "ERROR " << i << ": (" << vertices[i * 2] << ", " << vertices[i * 2 + 1] << ")\n";
            }
        }
    }


    // Check the location of the slopeValue variable
    GLint slopeValueLocation = glGetUniformLocation(shaderProgram, "slopeValue");
    if (slopeValueLocation == -1) {
        std::cout << "Failed to get the location of the slopeValue variable." << std::endl;
    }


    // Delete shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);



    // Background color
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    // Clear the back buffer and give it the new color
    glClear(GL_COLOR_BUFFER_BIT);
    // Swap the back buffer with the front buffer
    glfwSwapBuffers(window);



    // Swap the back buffer with the front buffer
    glfwSwapBuffers(window);



    // Main rendering loop
    while (!glfwWindowShouldClose(window)) {
        // Clear the back buffer and give it the new color
        glClear(GL_COLOR_BUFFER_BIT);

        glLineWidth(2);


// -------------------------------------------- 2D AND 3D DRAW METHODS ---------------------------------------
        drawGraphLines2D(VAO, numPoints, shaderProgram, slopeValueLocation, vertices);
        // drawGraphLines3D(VAO, numPoints*numPoints, shaderProgram, slopeValueLocation, vertices);
// -----------------------------------------------------------------------------------------------------------


        // Swap the back buffer with the front buffer
        glfwSwapBuffers(window);

        // Take care of all GLFW events
        glfwPollEvents();
    }

    // Delete window and terminate GLFW before closing the program
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}