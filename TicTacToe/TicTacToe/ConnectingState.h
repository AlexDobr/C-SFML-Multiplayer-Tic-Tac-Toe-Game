#pragma once

#include<SFML/Graphics.hpp>
#include"State.h"
#include "Game.h"

namespace TTT
{

	// called to establish connection between the host and client 

	class ConnectingState :public State
	{
	public:
		ConnectingState(GameDataRef data);

		void Init();

		void HandleInput();

		void Update(float dt);

		void Draw(float dt);

	private:

		GameDataRef _data;
		sf::Sprite _background;
		sf::Sprite _hostButton;
		sf::Sprite _joinButton;
	};

}

