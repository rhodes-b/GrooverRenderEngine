
#include "rendererV1.hpp"

// NEED INHERIT goover::Renderer publicly
class RenderTest: public groover::Renderer {

    groover::point recp1 = groover::point{ 0.0f, 0.0f};
    groover::point recp2 = groover::point{ 50.0f, 50.0f };
    // for the moving rec below


    bool Startup() override {
        std::cout << "overridden startup\n";
        return true;

    }

    bool Update(float deltaTime) override {

        this->SetColor(255, 0, 0);
        for(int i=0; i < winHeight; ++i) {
            this->DrawPoint(groover::point{ (float)i, (float)i } );
        }
         // this is moving rec
        // p1.x += 5 * deltaTime; p2.x += 5 * deltaTime;
        // do moving in keycallback but we dont know delta t
        this->DrawRec(recp1, recp2);
        // */
        this->SetColor(0, 255, 0);
        for(int i=0; i < winWidth-winHeight; ++i) {
            this->DrawPoint(groover::point{ (float)i+winHeight, (float)winHeight-i });
            // this->DrawPoint(i + winHeight, winHeight - i);
        }
        this->SetColor(0, 0, 255);
        groover::point p1 = groover::point{ 100.0f, 200.0f };
        groover::point p2 = groover::point{ 100.0f, 100.0f };
        groover::point p3 = groover::point{ 200.0f, 100.0f };
        this->DrawTriangle(p1, p2, p3);

        this->SetColor(0, 200, 255);
        // this only appears on screen when its big enough  below
        this->DrawRec(groover::point{ 1000.0f, winHeight/2.0f}, groover::point{ 1050.0f, winHeight/2.0f + 100 });
        this->SetColor(255, 255, 0);
        this->DrawCircle(groover::point{300.0f, 300.0f}, 50.0f);

        // make instance vars for
        // because we can access the stuff created in Setup() :(
        // point recP1 = point{ 0.0f, 0.0f };
        // point recP2 = point{ 50.0f, 50.0f };
        //this->SetColor(255, 0, 0);
        //this->DrawRec(recP1, recP2);
        //recP1.x += 5 * (1/60.0f); recP2.x += 5 * (1/60.0f);

        if(glfwGetKey(window, GLFW_KEY_A)) {
            // std::cout << "recp1.x: " << recp1.x << " recp2.x: " << recp2.x << "\n";
            recp1.x -= 50*deltaTime; recp2.x -= 50*deltaTime;
        }
        if(glfwGetKey(window, GLFW_KEY_D)) {
            recp1.x += 50*deltaTime; recp2.x += 50*deltaTime;
        }
        if(glfwGetKey(window, GLFW_KEY_W)) {
            recp1.y -= 50*deltaTime; recp2.y -= 50*deltaTime;
        }
        if(glfwGetKey(window, GLFW_KEY_S)) {
            recp1.y += 50*deltaTime; recp2.y += 50*deltaTime;
        }

        return true;
    }

};

int main() {
    std::cout << "starting renderer\n";
    RenderTest render;
    render.CreateWin("title", 640, 480, NULL, NULL);
    render.Run();

    return 0;
}