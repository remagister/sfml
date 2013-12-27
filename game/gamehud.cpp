
#include "gamehud.h"


// IconButton class inline definitions //

template<class arg>
inline void IconButton<arg>::Press(){
	_ico->setColor(Styles::Colors::BLACK);
}

template <class arg>
inline void IconButton<arg>::Highlight(){
	_ico->setColor(Styles::Colors::WHITE_GHOST);
}

template <class arg>
inline bool IconButton<arg>::Release(float x, float y){
	if(contains(x,y) && GetState() == States::PRESSED){
		_ico->setColor(Styles::Colors::WHITE_GHOST);
		return true;
	}
	else _ico->setColor(Styles::Colors::WHITE);
	return false;
}

template <class arg>
inline void IconButton<arg>::draw(sf::RenderTarget& target, sf::RenderStates state)const{
	target.draw(_ico);

}