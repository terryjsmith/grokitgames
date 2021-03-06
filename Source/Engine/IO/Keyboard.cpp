
#include <IO/Keyboard.h>
#include <IO/InputSystem.h>
#include <GLFW/glfw3.h>
#include <Core/Application.h>
#include <Core/MessageSystem.h>
#include <IO/Window.h>
#include <IO/KeyboardMessage.h>
#include <IO/Command.h>

Keyboard::Keyboard() {
    m_type = INPUTDEVICE_KEYBOARD;
}

// GLFW callback functions
void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Get our input system
    InputSystem* ioSystem = GetSystem<InputSystem>();
    
    // Find our keyboard device
    Keyboard* keyboard = dynamic_cast<Keyboard*>(ioSystem->GetInputDevice(INPUTDEVICE_KEYBOARD));
    
    // Dispatch event
    keyboard->ProcessAction(key, action);
}

void Keyboard::Initialize() {
    // Get our window
    Window* window = Window::GetInstance();
    GLFWwindow* glfwWindow = (GLFWwindow*)window->GetWindowHandle();
    
    glfwSetKeyCallback(glfwWindow, KeyboardCallback);

    // Set up key mappings from GLFW
    m_keyMappings[KEY_0] = GLFW_KEY_0;
    m_keyMappings[KEY_1] = GLFW_KEY_1;
    m_keyMappings[KEY_2] = GLFW_KEY_2;
    m_keyMappings[KEY_3] = GLFW_KEY_3;
    m_keyMappings[KEY_4] = GLFW_KEY_4;
    m_keyMappings[KEY_5] = GLFW_KEY_5;
    m_keyMappings[KEY_6] = GLFW_KEY_6;
    m_keyMappings[KEY_7] = GLFW_KEY_7;
    m_keyMappings[KEY_8] = GLFW_KEY_8;
    m_keyMappings[KEY_9] = GLFW_KEY_9;
    m_keyMappings[KEY_A] = GLFW_KEY_A;
    m_keyMappings[KEY_B] = GLFW_KEY_B;
    m_keyMappings[KEY_C] = GLFW_KEY_C;
    m_keyMappings[KEY_D] = GLFW_KEY_D;
    m_keyMappings[KEY_E] = GLFW_KEY_E;
    m_keyMappings[KEY_F] = GLFW_KEY_F;
    m_keyMappings[KEY_G] = GLFW_KEY_G;
    m_keyMappings[KEY_H] = GLFW_KEY_H;
    m_keyMappings[KEY_I] = GLFW_KEY_I;
    m_keyMappings[KEY_J] = GLFW_KEY_J;
    m_keyMappings[KEY_K] = GLFW_KEY_K;
    m_keyMappings[KEY_L] = GLFW_KEY_L;
    m_keyMappings[KEY_M] = GLFW_KEY_M;
    m_keyMappings[KEY_N] = GLFW_KEY_N;
    m_keyMappings[KEY_O] = GLFW_KEY_O;
    m_keyMappings[KEY_P] = GLFW_KEY_P;
    m_keyMappings[KEY_Q] = GLFW_KEY_Q;
    m_keyMappings[KEY_R] = GLFW_KEY_R;
    m_keyMappings[KEY_S] = GLFW_KEY_S;
    m_keyMappings[KEY_T] = GLFW_KEY_T;
    m_keyMappings[KEY_U] = GLFW_KEY_Y;
    m_keyMappings[KEY_V] = GLFW_KEY_V;
    m_keyMappings[KEY_W] = GLFW_KEY_W;
    m_keyMappings[KEY_X] = GLFW_KEY_X;
    m_keyMappings[KEY_Y] = GLFW_KEY_Y;
    m_keyMappings[KEY_Z] = GLFW_KEY_Z;
    m_keyMappings[KEY_TAB] = GLFW_KEY_TAB;
    m_keyMappings[KEY_LEFT_SHIFT] = GLFW_KEY_LEFT_SHIFT;
    m_keyMappings[KEY_RIGHT_SHIFT] = GLFW_KEY_RIGHT_SHIFT;
    m_keyMappings[KEY_LEFT_CTRL] = GLFW_KEY_LEFT_CONTROL;
    m_keyMappings[KEY_RIGHT_CTRL] = GLFW_KEY_RIGHT_CONTROL;
    m_keyMappings[KEY_LEFT_ALT] = GLFW_KEY_LEFT_ALT;
    m_keyMappings[KEY_RIGHT_ALT] = GLFW_KEY_RIGHT_ALT;
    m_keyMappings[KEY_SPACE] = GLFW_KEY_SPACE;
    m_keyMappings[KEY_ESCAPE] = GLFW_KEY_ESCAPE;
    m_keyMappings[KEY_BACKSPACE] = GLFW_KEY_BACKSPACE;
    m_keyMappings[KEY_ENTER] = GLFW_KEY_ENTER;
    m_keyMappings[KEY_EQUALS] = GLFW_KEY_EQUAL;
    m_keyMappings[KEY_MINUS] = GLFW_KEY_MINUS;
    m_keyMappings[KEY_LEFT_SQUARE_BRACKET] = GLFW_KEY_LEFT_BRACKET;
    m_keyMappings[KEY_RIGHT_SQUARE_BRACKET] = GLFW_KEY_RIGHT_BRACKET;
    m_keyMappings[KEY_BACK_SLASH] = GLFW_KEY_BACKSLASH;
    m_keyMappings[KEY_FORWARD_SLASH] = GLFW_KEY_SLASH;
    m_keyMappings[KEY_COMMA] = GLFW_KEY_COMMA;
    m_keyMappings[KEY_PERIOD] = GLFW_KEY_PERIOD;
    m_keyMappings[KEY_INSERT] = GLFW_KEY_INSERT;
    m_keyMappings[KEY_DELETE] = GLFW_KEY_DELETE;
    m_keyMappings[KEY_HOME] = GLFW_KEY_HOME;
    m_keyMappings[KEY_END] = GLFW_KEY_END;
    m_keyMappings[KEY_PAGE_UP] = GLFW_KEY_PAGE_UP;
    m_keyMappings[KEY_PAGE_DOWN] = GLFW_KEY_PAGE_DOWN;
    m_keyMappings[KEY_PRINT_SCREEN] = GLFW_KEY_PRINT_SCREEN;
    m_keyMappings[KEY_UP] = GLFW_KEY_UP;
    m_keyMappings[KEY_DOWN] = GLFW_KEY_DOWN;
    m_keyMappings[KEY_LEFT] = GLFW_KEY_LEFT;
    m_keyMappings[KEY_RIGHT] = GLFW_KEY_RIGHT;
    m_keyMappings[KEY_NUMPAD_PLUS] = GLFW_KEY_KP_ADD;
    m_keyMappings[KEY_NUMPAD_0] = GLFW_KEY_KP_0;
    m_keyMappings[KEY_NUMPAD_1] = GLFW_KEY_KP_1;
    m_keyMappings[KEY_NUMPAD_2] = GLFW_KEY_KP_2;
    m_keyMappings[KEY_NUMPAD_3] = GLFW_KEY_KP_3;
    m_keyMappings[KEY_NUMPAD_4] = GLFW_KEY_KP_4;
    m_keyMappings[KEY_NUMPAD_5] = GLFW_KEY_KP_5;
    m_keyMappings[KEY_NUMPAD_6] = GLFW_KEY_KP_6;
    m_keyMappings[KEY_NUMPAD_7] = GLFW_KEY_KP_7;
    m_keyMappings[KEY_NUMPAD_8] = GLFW_KEY_KP_8;
    m_keyMappings[KEY_NUMPAD_9] = GLFW_KEY_KP_9;
    m_keyMappings[KEY_FUNC_F1] = GLFW_KEY_F1;
    m_keyMappings[KEY_FUNC_F2] = GLFW_KEY_F2;
    m_keyMappings[KEY_FUNC_F3] = GLFW_KEY_F3;
    m_keyMappings[KEY_FUNC_F4] = GLFW_KEY_F4;
    m_keyMappings[KEY_FUNC_F5] = GLFW_KEY_F5;
    m_keyMappings[KEY_FUNC_F6] = GLFW_KEY_F6;
    m_keyMappings[KEY_FUNC_F7] = GLFW_KEY_F7;
    m_keyMappings[KEY_FUNC_F8] = GLFW_KEY_F8;
    m_keyMappings[KEY_FUNC_F9] = GLFW_KEY_F9;
    m_keyMappings[KEY_FUNC_F10] = GLFW_KEY_F10;
    m_keyMappings[KEY_FUNC_F11] = GLFW_KEY_F11;
    m_keyMappings[KEY_FUNC_F12] = GLFW_KEY_F12;
}

void Keyboard::ProcessAction(int key, int action) {
    InputSystem* inputSystem = GetSystem<InputSystem>();
    MessageSystem* messageSystem = GetSystem<MessageSystem>();
    
    if (action == GLFW_PRESS) {
        for (int i = 0; i < KEY_LAST; i++) {
            if (m_keyMappings[i] == key) {
                // Send a key press message
                messageSystem->Broadcast(new Message("KEY_PRESS", new KeyboardMessage(i, KEY_PRESS)));

                // Send an applicable command message
                std::string command = inputSystem->FindInputMapping(this, i);
                if (command.empty() == false) {
                    Command::Start(command);
                }

                // Set key state
                m_buttons[i] = true;
                break;
            }
        }
    }
    
    if (action == GLFW_RELEASE) {
        for (int i = 0; i < KEY_LAST; i++) {
            if (m_keyMappings[i] == key) {
                // Send key release message
                messageSystem->Broadcast(new Message("KEY_RELEASE", new KeyboardMessage(i, KEY_RELEASE)));

                // Send an applicable command message
                std::string command = inputSystem->FindInputMapping(this, i);
                if (command.empty() == false) {
                    Command::End(command);
                }

                // Set key state back
                m_buttons[i] = false;
                break;
            }
        }
    }
}
