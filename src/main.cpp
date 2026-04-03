#include "config.h"
#include "camera.h"
#include "axes.h"
#include "point_cloud.h"
#include "quantum.h"
#include "sampler.h"

Camera* g_camera = nullptr;

void mouse_button_callback(GLFWwindow*, int button, int action, int){
    if(g_camera) g_camera->onMouseButton(button, action);
}
void cursor_pos_callback(GLFWwindow*, double x, double y){
    if(g_camera) g_camera->onMouseMove(x, y);
}
void scroll_callback(GLFWwindow*, double x, double y){
    if(g_camera) g_camera->onScroll(y);
}

// --- Compilation shader ---
unsigned int make_module(const std::string& filepath, unsigned int type){
    std::ifstream file(filepath);
    if(!file.is_open())
        throw std::runtime_error("Impossible d'ouvrir : " + filepath);

    std::stringstream ss;
    ss << file.rdbuf();
    std::string src = ss.str();

    // DEBUG — affiche le contenu lu
    //std::cerr << "=== Shader lu : " << filepath << " ===" << std::endl;
    //std::cerr << src << std::endl;
    //std::cerr << "=== Fin ===" << std::endl;

    const char* csrc = src.c_str();

    unsigned int mod = glCreateShader(type);
    glShaderSource(mod, 1, &csrc, NULL);
    glCompileShader(mod);

    int ok;
    glGetShaderiv(mod, GL_COMPILE_STATUS, &ok);
    if(!ok){
        char log[1024];
        glGetShaderInfoLog(mod, 1024, NULL, log);
        throw std::runtime_error(std::string("Shader error:\n") + log);
    }
    return mod;
}

unsigned int make_shader(const std::string& vert, const std::string& frag){
    unsigned int vs = make_module(vert, GL_VERTEX_SHADER);
    unsigned int fs = make_module(frag, GL_FRAGMENT_SHADER);

    unsigned int prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    int ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if(!ok){
        char log[1024];
        glGetProgramInfoLog(prog, 1024, NULL, log);
        throw std::runtime_error(std::string("Link error:\n") + log);
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}



int main(){
    if(!glfwInit()){
        std::cerr << "Erreur : GLFW init failed" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const float W = 800.0f, H = 600.0f;
    GLFWwindow* window = glfwCreateWindow((int)W, (int)H, "Qosmos", NULL, NULL);

    if(!window){
        std::cerr << "Erreur : fenêtre GLFW non créée" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "Erreur : GLAD init failed" << std::endl;
        glfwTerminate();
        return -1;
    }

    int fw, fh;
    glfwGetFramebufferSize(window, &fw, &fh);
    glViewport(0, 0, fw, fh);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);

    Camera camera(W / H);
    g_camera = &camera;
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetScrollCallback(window, scroll_callback);

    unsigned int shader_points = make_shader(
    "../src/shaders/vertex.glsl",
    "../src/shaders/fragment.glsl"
    );
    unsigned int shader_axes = make_shader(
        "../src/shaders/vertex_axes.glsl",
        "../src/shaders/fragment_axes.glsl"
    );

    Axes axes;
    PointCloud cloud;

    // Paramètres de l'orbitale
    int n = 1, l = 0, m = 0;
    int N = 50000;
    bool need_regen = true;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> colors;
    std::vector<float>     alphas;

    Sampler::generate(n, l, m, N, positions, colors, alphas);
    cloud.upload(positions, colors, alphas);

    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        camera.update();

        static int prev_r = GLFW_RELEASE;
        int cur_r = glfwGetKey(window, GLFW_KEY_R);
        if(cur_r == GLFW_PRESS && prev_r == GLFW_RELEASE)
            camera.autoRotate = !camera.autoRotate;
        prev_r = cur_r;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.getView();
        glm::mat4 proj = camera.getProjection();

        // --- Points ---
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        glUseProgram(shader_points);
        glUniformMatrix4fv(glGetUniformLocation(shader_points, "view"),
            1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader_points, "projection"),
            1, GL_FALSE, glm::value_ptr(proj));
        cloud.draw();

        // --- Axes ---
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
        glClear(GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_axes);
        glUniformMatrix4fv(glGetUniformLocation(shader_axes, "view"),
            1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader_axes, "projection"),
            1, GL_FALSE, glm::value_ptr(proj));
        //axes.draw();

        if(need_regen){
            positions.clear(); colors.clear(); alphas.clear();
            Sampler::generate(n, l, m, N, positions, colors, alphas);
            cloud.upload(positions, colors, alphas);
            // Met à jour le titre de la fenêtre
            std::string title = "Qosmos — n=" + std::to_string(n)
                            + " l=" + std::to_string(l)
                            + " m=" + std::to_string(m);
            glfwSetWindowTitle(window, title.c_str());
            need_regen = false;
        }

        // Gestion clavier — détection sur front montant
        static int prev_up    = GLFW_RELEASE;
        static int prev_down  = GLFW_RELEASE;
        static int prev_left  = GLFW_RELEASE;
        static int prev_right = GLFW_RELEASE;
        static int prev_u     = GLFW_RELEASE;
        static int prev_d_key = GLFW_RELEASE;

        int cur_up    = glfwGetKey(window, GLFW_KEY_UP);
        int cur_down  = glfwGetKey(window, GLFW_KEY_DOWN);
        int cur_left  = glfwGetKey(window, GLFW_KEY_LEFT);
        int cur_right = glfwGetKey(window, GLFW_KEY_RIGHT);
        int cur_u     = glfwGetKey(window, GLFW_KEY_U);
        int cur_d_key = glfwGetKey(window, GLFW_KEY_D);

        // Flèche HAUT/BAS — change n
        if(cur_up == GLFW_PRESS && prev_up == GLFW_RELEASE){
            n = std::min(n + 1, 6);
            l = std::min(l, n - 1);
            m = glm::clamp(m, -l, l);
            need_regen = true;
        }
        if(cur_down == GLFW_PRESS && prev_down == GLFW_RELEASE){
            n = std::max(n - 1, 1);
            l = std::min(l, n - 1);
            m = glm::clamp(m, -l, l);
            need_regen = true;
        }

        // Flèche DROITE/GAUCHE — change l
        if(cur_right == GLFW_PRESS && prev_right == GLFW_RELEASE){
            l = std::min(l + 1, n - 1);
            m = glm::clamp(m, -l, l);
            need_regen = true;
        }
        if(cur_left == GLFW_PRESS && prev_left == GLFW_RELEASE){
            l = std::max(l - 1, 0);
            m = glm::clamp(m, -l, l);
            need_regen = true;
        }

        // U/D — change m
        if(cur_u == GLFW_PRESS && prev_u == GLFW_RELEASE){
            m = std::min(m + 1, l);
            need_regen = true;
        }
        if(cur_d_key == GLFW_PRESS && prev_d_key == GLFW_RELEASE){
            m = std::max(m - 1, -l);
            need_regen = true;
        }

        prev_up    = cur_up;
        prev_down  = cur_down;
        prev_left  = cur_left;
        prev_right = cur_right;
        prev_u     = cur_u;
        prev_d_key = cur_d_key;


    glfwSwapBuffers(window);
    }

    glDeleteProgram(shader_points);
    glDeleteProgram(shader_axes);
    glfwTerminate();
    return 0;
}