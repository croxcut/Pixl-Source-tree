PIXL ENGINE 0.1.0

This project is my hobby project not gonna be a big open source project like godot(obviously)

this is supposed to be written in C but i choose not to write it in C entirely

i just want to share my progress for creating my own game engine :D*.

for now it sucks because of the boilerplate shit you have to deal with, im sorry for that. Im still working behind curtains on the editor and stuff, and i will add them to this repository
later down the line when i fixed the re-occuring issues :D*

and also just for my portfolio :D* 

Here is a little sample snippet of the setup :D*
```cpp

#include "main/engine.h"        // <-- important include :D*

class DemoApp : public IAppLogic {

    void init() {
        // Initialize Game Objects (create life :D*)
    }

    void tick(const f32& dt) {
        // (essentially an update function) dt <- delta time (fixed dt)
    }

    void render() {
        // Render shit (this is where you draw your smiley face :D*)
    }

    void cleanup() {
        // Cleanup shit (destroy everything essentially)
    }

};

int main() {

    DemoApp app;            // <-- create an app object
    Engine engine(app);     // <-- pass the app to the engine
    engine.start();         // <-- start the engine :D*

}

```

see how stupid that is? :D* for real. only insane people will use this if they want to

are you insane??...