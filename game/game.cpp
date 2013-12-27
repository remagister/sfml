// game.cpp: определяет точку входа для консольного приложения.
//

#include "misc.h"
#include "private.h"
#include "gamehud.h"
using namespace std;
string TITLE = "Title";							// window title

void func(const int& t){
	cout << "int button pressed, code: " << t << "\n";
}
 
void func(){
	cout << "void button pressed\n";
}

int main()
{
	ResourceManager res("Resource/script.txt");
	/*_Button_base<int, int> *butn = r.getButton<int,int>("Resource/Font/buttons.txt", "button", LABEL, func);*/
	sf::Sprite* norm = res.getSprite("start_normal");
	sf::Sprite* prs = res.getSprite("start_pressed");
	sf::Sprite* hover = res.getSprite("start_hover");
	sf::Sprite* enorm = res.getSprite("exit_normal");
	sf::Sprite* eprs = res.getSprite("exit_pressed");
	sf::Sprite* ehover = res.getSprite("exit_hover");
	HUD hud;
	float x= 10, y=10; int count=1;
	for(int i=0; i<70; i++){
		if(y > 600){
			y = 10;
			x += 205;
			if(x > 1100) break;
		}
		hud.AddButton<int>(Button<int>(sf::FloatRect(x,y+=55,200,50),norm,hover,prs,Event<void,int>(func),count++));
		hud.AddButton<int>(Button<int>(sf::FloatRect(x,y+=55,200,50),enorm,ehover,eprs,Event<void,int>(func),count));
	}
	sf::Event e;
	sf::Mouse mouse;
	sf::RenderWindow window(sf::VideoMode(1280,720), TITLE);

	while(window.isOpen()){

		while(window.pollEvent(e)){
			if(e.type == sf::Event::Closed)
				window.close();
			hud.Process(mouse, window, e);
		}

		hud.Draw(window);
		window.display();
	}

	return 0;
}

