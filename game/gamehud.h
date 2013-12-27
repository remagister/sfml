#pragma once

#ifndef GAMEHUD_H
#define GAMEHUD_H

#ifndef INCLUDES_H
#include "includes.h"
#endif // includes_h
#ifndef EVENT_H
#include "event.h"
#endif

namespace States{
	// enumeration of button states
	enum ButtonState { USUAL = -1, HIGHLIGHTED, PRESSED};
}


// convert sf::Vector2<int> to sf::Vector2<float>
sf::Vector2f toVector2f(sf::Vector2i& t){
	sf::Vector2f ret;
	ret.x = t.x;
	ret.y = t.y;
	return ret;
}

namespace Styles{
	enum Align{
		NOALIGN, LEFT, CENTER, RIGHT
	};
	enum Size{
		NOSIZE, TINY, SMALL, NORMAL, BIG, HUGE, ENORMOUS
	};
	namespace Colors{
		static sf::Color RED(255,0,0,255);
		static sf::Color GREEN(0,255,0,255);
		static sf::Color BLUE(0,0,255,255);
		static sf::Color WHITE(255,255,255,255);
		static sf::Color WHITE_GHOST(248,248,255,255);
		static sf::Color WHITE_SNOW(255,250,250,255);
		static sf::Color BLACK(0,0,0,255);
		static sf::Color YELLOW(255,255,0,255);
		static sf::Color MAGENTA(255,0,255,255);
		static sf::Color CYAN(0,255,255,255);
		static sf::Color GRAY(128,128,128,255);
		static sf::Color SILVER(192,192,192,255);
		static sf::Color GREEN_DARK(0,128,0,255);
		static sf::Color RED_DARK(128,0,0,255);
		static sf::Color BLUE_DARK(0,0,128,255);
		static sf::Color ORANGE(255,165,0,255);
		static sf::Color GOLD(255,215,0,255);
		static sf::Color GOLDEN_ROD(218,165,32,255);
		static sf::Color GREEN_LIME(50,205,50,255);
		static sf::Color BLUE_MIDNIGHT(25,25,112,255);
		static sf::Color BLUE_VIOLET(138,43,226,255);
		static sf::Color BLUE_INDIGO(75,0,130,255);
		static sf::Color VIOLET_DARK(148,0,211,255);
		static sf::Color PINK_DEEP(255,20,147,255);
		static sf::Color VIOLET_RED(199,21,133,255);
		static sf::Color BLUE_STATE(106,90,205,255);
		static sf::Color BROWN(139,69,19,255);
		static sf::Color GRAY_STATE(112,128,144,255);
		static sf::Color KHAKI(240,230,140,255);
	};
	class Fonts{
	public:
		static sf::Font HUDFONT;
	};
}

class InfoBar : public sf::FloatRect{
public:
	InfoBar() :Rect(){
		_background = 0;
	}
	InfoBar(const sf::Vector2f& Pos, const sf::Vector2f& Size) : Rect(Pos, Size){
		_background = 0;
	}
	InfoBar(float x, float y, float w, float h) : Rect(x,y,w,h){
		_background = 0;
	}
	InfoBar(const sf::Vector2f& pos, const sf::Vector2f& size, const sf::Sprite &bg) : Rect(pos, size){
		_background = new sf::Sprite(bg);
	}
	InfoBar(float x, float y, float w, float h, const sf::Sprite &bg) : Rect(x,y,w,h){
		_background = new sf::Sprite(bg);
	}

	void Move(float x, float y){
		left += x;
		top += y;
		for(int i=0; i<_text_inside.size(); i++)
			_text_inside[i]->move(x,y);
		if(_background) _background->move(x,y);
	}
	void Setpos(float x, float y){
		Move(x - left, y - top);
	}

	void Draw(sf::RenderWindow &window){
		if(_background) window.draw(*_background);
		for(unsigned i=0; i<_text_inside.size(); i++)
			window.draw(*_text_inside[i]);
	}

	void SetBg(const sf::Sprite& spr){
		if(_background) delete _background;

		_background = new sf::Sprite(spr);
		sf::FloatRect b = _background->getGlobalBounds();
		_background->scale(width/b.width, height/b.height);
	}
	// str is for placement textline in a correct order 
	void AddLabel(sf::Text* t, Styles::Align al, Styles::Size sz, unsigned str = 1){
		using namespace Styles;
		t->setScale(1,1);
		switch(sz){
		case TINY:	t->setCharacterSize(5); break;
		case SMALL:	t->setCharacterSize(10); break;
		case NORMAL: t->setCharacterSize(20); break;
		case BIG: t->setCharacterSize(40); break;
		case Size::HUGE:	t->setCharacterSize(60); break;
		case ENORMOUS:	t->setCharacterSize(80);
		}
		switch(al){
		case LEFT:{
			int s = (int) height / t->getLocalBounds().height;
			t->setPosition(left+(left/20), top+(s*str));
			break;
				  }
		case RIGHT:{
			float s = height / t->getLocalBounds().height;
			t->setPosition(left+(width-left/20), top+(s*str));
			break;
				   }
		case CENTER:{
			float s = height / t->getLocalBounds().height;
			t->setPosition(left+(width/2-t->getLocalBounds().width/2), top+(s*str));
					}
		}
		_text_inside.push_back(t);
	}

	~InfoBar(){
		if(_background) delete _background;
	}
private:
	std::vector<sf::Text*> _text_inside;
	sf::Sprite* _background;
};

// makes any derived class - pressable
__interface Pressable{
public:
	virtual void Press() = 0;
	virtual void Highlight() =0;
	virtual bool Release(float x, float y) = 0;
	// mouse hover - return 0
	// mouse away - return -1
	// mouse hover + pressed - return 1
	// may not be correct ^
	virtual int ProcessMouse(const sf::Mouse& mouse, const sf::Window& relative, const sf::Event& evnt) = 0;
	// fulscreen version of ProcessMouse()
	virtual int ProcessMouse(const sf::Mouse& m, const sf::Event& evnt) = 0;
	virtual States::ButtonState GetState()const = 0;
};

//======================================================= abstract button base classes/ template specialization
template <class Ret, class Arg>
// button abstract class
class _Button_base : public sf::FloatRect, public sf::Transformable, public sf::Drawable, public Pressable{
public:
	_Button_base(){}
	_Button_base(const sf::FloatRect& rect, const Event<Ret, Arg>& evnt): FloatRect(rect), _myevent(evnt){
		_binder = BindEvent(evnt.GetEvent(), Arg(0));
	}

	virtual Ret Execute(){
		return _binder.Exec();
	}
	sf::FloatRect Rect()const{
		return sf::FloatRect(left, top, width, height);
	}

	virtual void Bind(Ret (*evn)(const Arg&), const Arg& arg){
		_myevent = evn;
		_binder.Bind(evn, arg);
	}
	virtual void Bind(const Arg& arg){
		_binder.Bind(arg);
	}
protected:
	Event<Ret, Arg> _myevent;
	EBinder<Ret, Arg> _binder;
};

template <class Ret>
class _Button_base<Ret, void> : public sf::FloatRect, public sf::Transformable, public sf::Drawable, public Pressable{
public:
	_Button_base(){}
	_Button_base(const sf::FloatRect& rect, const Event<Ret, void>& evnt): sf::Rect<float>(rect), _myevent(evnt), _binder(evnt.GetEvent()){}

	virtual Ret Execute(){
		return _binder.Exec();
	}

	virtual void Bind(Ret (*evn)()){
		_myevent = evn;
		_binder.Bind(evn);
	}
	sf::FloatRect Rect()const{
		return sf::FloatRect(left, top, width, height);
	}
protected:
	Event<Ret, void> _myevent;
	EBinder<Ret, void> _binder;
};

template <class Arg>
class _Button_base<void,Arg> : public sf::FloatRect, public sf::Transformable, public sf::Drawable, public Pressable{
public:
	_Button_base(){}
	_Button_base(const sf::FloatRect& rect, const Event<void, Arg>& evnt): sf::Rect<float>(rect), _myevent(evnt){
		_binder = BindEvent(evnt.GetEvent(), Arg(0));
	}

	virtual void Execute(){
		_binder.Exec();
	}
	virtual void Bind(void (*evn)(const Arg&), const Arg& arg){
		_myevent = evn;
		_binder.Bind(evn, arg);
	}
	virtual void Bind(const Arg& arg){
		_binder.Bind(arg);
	}
	sf::FloatRect Rect()const{
		return sf::FloatRect(left, top, width, height);
	}
protected:
	Event<void, Arg> _myevent;
	EBinder<void, Arg> _binder;
};

template <>
class _Button_base<void, void> : public sf::FloatRect, public sf::Transformable, public sf::Drawable, public Pressable{
public:
	_Button_base(){}
	_Button_base(const sf::FloatRect& rect, const Event<void,void>& evnt): sf::Rect<float>(rect), _myevent(evnt), _binder(evnt.GetEvent()){}

	virtual void Execute(){
		_binder.Exec();
	}
	virtual void Bind(void (*evn)()){
		_myevent = evn;
		_binder.Bind(evn);
	}

	sf::FloatRect Rect()const{
		return sf::FloatRect(left, top, width, height);
	}
protected:
	Event<void,void> _myevent;
	EBinder<void, void> _binder;
};

////////////////////////==========================================	real button classes
template <class EventArg>
class Button: public _Button_base<void, EventArg>{
public:
	typedef typename EventArg _arg;

	Button(){
		_usual = _pressed = _hited = 0;
	}
	Button(const sf::FloatRect& posit, sf::Sprite* face, sf::Sprite* hl, sf::Sprite* pressed,const Event<void, _arg>& evnt, const _arg& arg)
		: _Button_base<void,_arg>(posit, evnt)
	{
		_binder.Bind(arg);
		_usual = _pressed = _hited = 0;
		_scopy(_usual, face);
		_usual->setPosition(posit.left, posit.top);
		_scopy(_pressed, pressed);
		_pressed->setPosition(posit.left, posit.top);
		_scopy(_hited, hl);
		_hited->setPosition(posit.left, posit.top);
		_cur(_usual);
	}
	Button(const Button& t) : _Button_base<void, _arg>(t.Rect(),t._myevent){
		_binder = t._binder;
		_usual = _pressed = _hited = 0;
		_scopy(_usual, t._usual);
		_scopy(_pressed, t._pressed);
		_scopy(_hited, t._hited);
		_cur(t._current);
	}
	virtual void Press(){
		_cur(_pressed);
	}
	virtual void Highlight(){
		_cur(_hited);
	}

	virtual bool Release(float x, float y){
		if(_Button_base<void,_arg>::contains(x,y)){
			_cur(_hited);
			return true;
		}
		else{
			_cur(_usual);
			return false;
		}
	}

	virtual int ProcessMouse(const sf::Mouse& mouse, const sf::Window& relative, const sf::Event& evnt){
		if(GetState() == States::PRESSED){
			if(evnt.type == sf::Event::MouseButtonReleased && Release(mouse.getPosition(relative).x, mouse.getPosition(relative).y))
				_binder.Exec();
		}
		if(GetState() == States::HIGHLIGHTED && !_Button_base<void, _arg>::contains(toVector2f(mouse.getPosition(relative)))){
			_cur(_usual);
			return -1;
		}
		if(_Button_base<void, _arg>::contains(toVector2f(mouse.getPosition(relative)))) {
			if(mouse.isButtonPressed(sf::Mouse::Button::Left)){
				if(GetState() == States::HIGHLIGHTED) _cur(_pressed);
				return 1;
			}
			_cur(_hited);
			return 0;
		}
		else return -1;

	}
	virtual int ProcessMouse(const sf::Mouse& mouse, const sf::Event& evnt){
		if(GetState() == States::PRESSED){
			if(evnt.type == sf::Event::MouseButtonReleased && Release(mouse.getPosition().x, mouse.getPosition().y))
				_binder.Exec();
		}
		if(GetState() == States::HIGHLIGHTED && !_Button_base<void, _arg>::contains(toVector2f(mouse.getPosition()))){
			_cur(_usual);
			return -1;
		}
		if(_Button_base<void, _arg>::contains(toVector2f(mouse.getPosition()))) {
			if(mouse.isButtonPressed(sf::Mouse::Button::Left)){
				if(GetState() == States::HIGHLIGHTED) _cur(_pressed);
				return 1;
			}
			_cur(_hited);
			return 0;
		}
		else return -1;

	}
	virtual States::ButtonState GetState()const{
		using namespace States;
		return (_current == _usual? USUAL : (_current == _pressed ? PRESSED : HIGHLIGHTED) );
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates state)const{
		target.draw(*_current, state);
	}

	~Button(){
		if(_usual) delete _usual;
		if(_pressed) delete _pressed;
		if(_hited) delete _hited;
	}
private:
	void _cur(sf::Sprite* spr){
		_current = spr;
	}
	void _scopy(sf::Sprite* &dest, const sf::Sprite* src){
		if(dest) delete dest;
		if(src) dest = new sf::Sprite(*src);
	}
	sf::Sprite *_current;
	sf::Sprite *_usual;
	sf::Sprite *_pressed;
	sf::Sprite *_hited;
};
template <>
class Button<void> : _Button_base<void, void>{
public: Button(const sf::FloatRect& posit, sf::Sprite* face, sf::Sprite* hl, sf::Sprite* pressed,const Event<void, void>& evnt):
	_Button_base(posit,evnt)
	{
		_usual = _pressed = _hited = 0;
		_scopy(_usual, face);
		_usual->setPosition(posit.left, posit.top);
		_scopy(_pressed, pressed);
		_pressed->setPosition(posit.left, posit.top);
		_scopy(_hited, hl);
		_hited->setPosition(posit.left, posit.top);
		_cur(_usual);
	}
	Button(const Button& t) : _Button_base<void, void>(t.Rect(),t._myevent){
		_usual = _pressed = _hited = 0;
		_scopy(_usual, t._usual);
		_scopy(_pressed, t._pressed);
		_scopy(_hited, t._hited);
		_cur(t._current);
	}
	virtual void Press(){
		_cur(_pressed);
	}
	virtual void Highlight(){
		_cur(_hited);
	}

	virtual bool Release(float x, float y){
		if(_Button_base<void,void>::contains(x,y)){
			_cur(_hited);
			return true;
		}
		else{
			_cur(_usual);
			return false;
		}
	}

	virtual int ProcessMouse(const sf::Mouse& mouse, const sf::Window& relative, const sf::Event& evnt){
		if(GetState() == States::PRESSED){
			if(evnt.type == sf::Event::MouseButtonReleased && Release(mouse.getPosition(relative).x, mouse.getPosition(relative).y))
				_binder.Exec();
		}
		if(GetState() == States::HIGHLIGHTED && !_Button_base<void, void>::contains(toVector2f(mouse.getPosition(relative)))){
			_cur(_usual);
			return -1;
		}
		if(_Button_base<void, void>::contains(toVector2f(mouse.getPosition(relative)))) {
			if(mouse.isButtonPressed(sf::Mouse::Button::Left)){
				if(GetState() == States::HIGHLIGHTED) _cur(_pressed);
				return 1;
			}
			_cur(_hited);
			return 0;
		}
		else return -1;

	}

	virtual int ProcessMouse(const sf::Mouse& mouse, const sf::Event& evnt){
		if(GetState() == States::PRESSED){
			if(evnt.type == sf::Event::MouseButtonReleased && Release(mouse.getPosition().x, mouse.getPosition().y))
				_binder.Exec();
		}
		if(GetState() == States::HIGHLIGHTED && !_Button_base<void, void>::contains(toVector2f(mouse.getPosition()))){
			_cur(_usual);
			return -1;
		}
		if(_Button_base<void, void>::contains(toVector2f(mouse.getPosition()))) {
			if(mouse.isButtonPressed(sf::Mouse::Button::Left)){
				if(GetState() == States::HIGHLIGHTED) _cur(_pressed);
				return 1;
			}
			_cur(_hited);
			return 0;
		}
		else return -1;

	}

	virtual States::ButtonState GetState()const{
		using namespace States;
		return (_current == _usual? USUAL : (_current == _pressed ? PRESSED : HIGHLIGHTED) );
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates state)const{
		target.draw(*_current, state);
	}

	~Button(){
		if(_usual) delete _usual;
		if(_pressed) delete _pressed;
		if(_hited) delete _hited;
	}
private:
	void _cur(sf::Sprite* spr){
		_current = spr;
	}
	void _scopy(sf::Sprite* &dest, const sf::Sprite* src){
		if(dest) delete dest;
		if(src) dest = new sf::Sprite(*src);
	}
	sf::Sprite *_current;
	sf::Sprite *_usual;
	sf::Sprite *_pressed;
	sf::Sprite *_hited;
};

template <class EventArg>
class IconButton : public _Button_base<void, EventArg>{
public:
	typedef typename EventArg _arg;
	typedef IconButton<_arg> _myt;
	IconButton(){}
	IconButton(const sf::FloatRect& rect, const sf::Sprite& spr, const Event<void, _arg> &evn , const _arg& arg) :
		_Button_base<void, _arg>(rect, evn){
		_binder.Bind(arg);
		_ico = new sf::Sprite(spr);
	}
	// this version of ctor makes an IconButton from texture
	// where trect - texture rectangle
	// pos - rectangle of a button, placed anywhere on the screen
	IconButton(const sf::FloatRect& pos, const sf::FloatRect &trect, const sf::Texture& tex,const Event<void, _arg> &evn , const _arg& arg):
		_Button_base<void, _arg>(pos, evn){
		_ico = new sf::Sprite(tex, trect);
		_binder.Bind(arg);

	}

	virtual void Press();
	virtual void Highlight();
	virtual bool Release(float x, float y);
	virtual void ProcessMouse(const sf::Mouse& m, const sf::Window& win, const sf::Event& e);
	virtual void ProcessMouse(const sf::Mouse& m, const sf::Event& e);


	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	States::ButtonState GetState()const;

	~IconButton(){
		if(_ico) delete _ico;
	}
private:
	sf::Sprite* _ico;
	sf::FloatRect _rect;
};


class Storage{
public:
	Storage(){}
	template <class Type>
	Type* Push(const Type& what){
		Type* p = new Type(what);
		_cont.push_back((void*)p);
		return p;
	}
	void Clear(){
		for(unsigned i=0; i<_cont.size(); i++)
			delete _cont[i];
	}

	template <class Type>
	Type* Get(int x)const{
		return (Type*)(_cont[x]);
	}
	unsigned Size()const{
		return _cont.size();
	}
	~Storage(){
		for(unsigned i=0; i<_cont.size(); i++)
			delete _cont[i];
	}
private:
	std::vector<void*> _cont;
};

class HUD : public sf::Rect<float>{
public:
	HUD(sf::RenderWindow& windw){
	}
	HUD(){}

	template<class Type>
	void AddButton(Button<Type> &butn){
		Button<Type>* p = _mystor.Push<Button<Type>>(butn);
		_elems.push_back(dynamic_cast<Pressable*>(p));
		_drelems.push_back(dynamic_cast<sf::Drawable*>(p));
	}

	void Process(const sf::Mouse& mouse, const sf::Window& window, const sf::Event& e){
		for(std::vector<Pressable*>::iterator i= _elems.begin(); i!= _elems.end(); i++)
			(*i)->ProcessMouse(mouse, window, e);
	}
	void Draw(sf::RenderWindow& w){
		for(std::vector<sf::Drawable*>::iterator i= _drelems.begin(); i!= _drelems.end(); i++)
			w.draw(**i);
	}
	~HUD(){

	}
private:
	std::vector<Pressable*> _elems;
	std::vector<sf::Drawable*> _drelems;
	Storage _mystor;
};

#endif // gamehud_h