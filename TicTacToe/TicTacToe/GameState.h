#pragma once

#include<SFML/Graphics.hpp>
#include"State.h"
#include "Game.h"
#include <array>

namespace TTT
{

	class GameState :public State
	{
	public:
		GameState(GameDataRef data, bool first);

		void Init();

		void HandleInput();

		void Update(float dt);

		void Draw(float dt);

	private:

		void InitGridPieces();
		void CheckAndPlacePiece(sf::Vector2i touchpoint);
		void PostMessage(std::string);
		void CheckHasPlayerWon(int _turn);
		void Check3PiecesForMatch(int x1, int y1, int x2, int y2, int x3, int y3, int pieceToCheck);
		void UpdateInput();

		GameDataRef _data;
		sf::Sprite _background;
		sf::Sprite _pauseButton;
		sf::Sprite _gridSprite;
		sf::Sprite _gridPieces[3][3];
		sf::Clock _clock;
		sf::Packet _packet;
		sf::RectangleShape _chatBox;    
		sf::RectangleShape _messageBox;
		sf::Text _textInput;
		std::array<sf::Text, 20> _chatHistory; // variable that stores the maximum amount of chat history lines available for scrolling 
		std::string _rawInput;
		sf::Font _font;


		int _gridArray[3][3];
		int _turn;
		int _gameState;
		bool _focused; // variable used for setting focus on chat scrolling 
		int _lineFocus; // variable used for focusing on previous and future messages in the chat
	};

}