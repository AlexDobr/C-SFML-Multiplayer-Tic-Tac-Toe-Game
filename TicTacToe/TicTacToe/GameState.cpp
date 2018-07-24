#pragma once
#include "stdafx.h"
#include "GameState.h"
#include <iostream>
#include "DEFINITIONS.h"
#include <sstream>
#include "PauseState.h"
#include "GameOverState.h"
#include <cctype>

namespace TTT
{
	GameState::GameState(GameDataRef data, bool first) :_data(data), _focused(false)
	{
		if (first)
		{
			_gameState = STATE_PLAYING;
			_turn = PLAYER_PIECE;

		}
		
		if (!first)
		{
			_gameState = STATE_OTHER_PLAYING;
			_turn = OTHER_PIECE;

		}
	}

	void GameState::Init()
	{
		_data->socket.setBlocking(false);
		//_gameState = STATE_PLAYING;
		//_turn = PLAYER_PIECE;

		//chat box visuals

		_chatBox.setSize(sf::Vector2f(500, 170));
		_chatBox.setFillColor(sf::Color(255, 255, 255, 125)); // white, but transperent ;)
		_messageBox.setSize(sf::Vector2f(500, 35));
		_messageBox.setFillColor(sf::Color(255, 255, 255, 125)); 
		_rawInput = "";

		//load textures

		this->_data->assets.LoadTexture("Game Background", GAME_BACKGROUND_FILEPATH);
		this->_data->assets.LoadTexture("Pause Button", PAUSE_BUTTON);
		this->_data->assets.LoadTexture("Grid Sprite", GRID_SPRITE_FILEPATH);
		this->_data->assets.LoadTexture("X Piece", X_PIECE_FILEPATH );
		this->_data->assets.LoadTexture("O Piece", O_PIECE_FILEPATH);
		this->_data->assets.LoadTexture("X Win", X_WINNING_PIECE_FILEPATH);
		this->_data->assets.LoadTexture("O Win", O_WINNING_PIECE_FILEPATH);
		this->_data->assets.LoadFont("Chat Font",CHAT_FONT);

		//set textures

		_background.setTexture(this->_data->assets.GetTexture("Game Background"));
		_pauseButton.setTexture(this->_data->assets.GetTexture("Pause Button"));
		_gridSprite.setTexture(this->_data->assets.GetTexture("Grid Sprite"));
		_font = this->_data->assets.GetFont("Chat Font");

		//chat box text

		for (int line = 0; line < _chatHistory.size(); line++)
		{
			_chatHistory[line].setString("");
			_chatHistory[line].setCharacterSize(11);
			_chatHistory[line].setFont(_font);
			_chatHistory[line].setFillColor(sf::Color::Black);
		}

		_textInput.setString("");
		_textInput.setCharacterSize(11);
		_textInput.setFillColor(sf::Color::Black);
		_textInput.setFont(_font);
		
		_lineFocus = _chatHistory.size() - 1;

		//set positions
		
		this->_pauseButton.setPosition(this->_data->window.getSize().x - (this->_pauseButton.getLocalBounds().width), this->_pauseButton.getPosition().y);
		this->_gridSprite.setPosition((SCREEN_WIDTH / 2) - (_gridSprite.getGlobalBounds().width / 2), (SCREEN_HEIGHT / 2) - (_gridSprite.getGlobalBounds().height/ 2));

		_chatBox.setPosition((SCREEN_WIDTH / 2) - (_chatBox.getGlobalBounds().width / 2), (SCREEN_HEIGHT) - (_chatBox.getGlobalBounds().height)-70);
		_messageBox.setPosition((SCREEN_WIDTH / 2) - (_messageBox.getGlobalBounds().width / 2), (SCREEN_HEIGHT)-(_messageBox.getGlobalBounds().height)-70);

		_textInput.setPosition(_messageBox.getPosition().x + 10, _messageBox.getPosition().y + (_messageBox.getGlobalBounds().height /2) - (_textInput.getGlobalBounds().height));


		InitGridPieces();

		for (int x = 0; x < 3; x++)
		{
			for (int y = 0; y < 3; y++)
			{
				_gridArray[x][y]=EMPTY_PIECE;
			}
		}

	}

	void GameState::HandleInput()
	{
		sf::Event event;
		while (this->_data->window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				this->_data->window.close();
			}
			else if (event.type == sf::Event::GainedFocus)
			{
				_focused = true;
			}
			else if (event.type == sf::Event::LostFocus)
			{
				_focused = false;
			}

			
			//do not accept input when window is not focused
			if (_focused)
			{
				// chat scrolling
				if (event.type == sf::Event::MouseWheelMoved)
				{
					if (event.mouseWheel.delta > 0 && _lineFocus > 9)
						--_lineFocus;
					else if (event.mouseWheel.delta < 0 && _lineFocus < 19)
						++_lineFocus;

				}

				if (event.type == sf::Event::KeyPressed)
				{
					if (event.key.code == sf::Keyboard::Return)
					{
						//client side command
						PostMessage(_rawInput);

						//construct packet and send
						_packet << sf::Uint8(SEND_MESSAGE) << sf::String(_rawInput);
						
						if (_data->socket.send(_packet) != sf::Socket::Done)
						{
						}
						else
						{
							_packet.clear();
							std::cout << "Packet constructed: sending packet" << std::endl;
						}

						//reset chat input
						_rawInput = "";
					}
				}
				//anytime a text event is triggered
				else if (event.type == sf::Event::TextEntered)
				{
					//if it is an ASCII charcater and can be printed
					if (event.text.unicode < 128 && std::isprint(event.text.unicode))
					{
						_rawInput += static_cast<char>(event.text.unicode);
					}
				}

				// sprite clicks


				else if (this->_data->input.IsSpriteClicked(this->_gridSprite, sf::Mouse::Left, this->_data->window))
				{

					if (STATE_PLAYING == _gameState)
					{
						//client side command
						this->CheckAndPlacePiece(this->_data->input.GetMousePosition(this->_data->window));

						//construct packet
						_packet << sf::Int8(PLACE_PIECE) << sf::Int32(this->_data->input.GetMousePosition(this->_data->window).x) << sf::Int32(this->_data->input.GetMousePosition(this->_data->window).y);

						if (_data->socket.send(_packet) != sf::Socket::Done)
						{
						}
						else
						{
							_packet.clear();
							std::cout << "Packet constructed: sending packet" << std::endl;
						}
					}
				}
			}
		}
	}

	void GameState::Update(float dt)
	{
		if (STATE_WON == _gameState || STATE_LOSE == _gameState || STATE_DRAW == _gameState)
		{
			if (this->_clock.getElapsedTime().asSeconds() > TIME_BEFORE_SHOWING_GAME_OVER)
			{
				this->_data->machine.AddState(StateRef(new GameOverState(_data)), true);

				//game over so disconnect from host
				_data->socket.disconnect();
			}
		}
				
		sf::Int8 packetID;
		packetID = -1;
		//std::cout << "awaiting packet" << std::endl;

		if (_data->socket.receive(_packet) != sf::Socket::Done)
		{
			//std::cout << "Error receiving packet" << std::endl;
		}
		else
			_packet >> packetID;
		//std::cout << "Packet Received" << std::endl;

		if (STATE_OTHER_PLAYING == _gameState)
		{
			if (packetID == PLACE_PIECE)  //if the other player has placed a piece
			{
				sf::Vector2i otherMove;

				if (_packet >> otherMove.x >> otherMove.y)  // temporarily storing the input from the piece placed by the other player
				{
					this->CheckAndPlacePiece(otherMove);

					_packet.clear();
				}
				else
				{
					std::cout << "Packet could not be processed" << std::endl;
				}
			}
		}

		if (packetID == SEND_MESSAGE) //if other player has sent a message
		{
			sf::String msg;
			if (_packet >> msg)
			{
				this->PostMessage(msg.toAnsiString());
				_packet.clear();
			}
			else
			{
				std::cout << "Packet could not be processed" << std::endl;
			}
		}

		UpdateInput(); //update input line rendering
	}

	void GameState::Draw(float dt)
	{
		this->_data->window.clear();

		this->_data->window.draw(_background);
		this->_data->window.draw(_pauseButton);
		this->_data->window.draw(_gridSprite);
		this->_data->window.draw(_chatBox);
		this->_data->window.draw(_messageBox);

		for (int x = 0; x < 3; x++)
		{
			for (int y = 0; y < 3; y++)
			{
				this->_data->window.draw(this->_gridPieces[x][y]);
			}
		}

		for (unsigned int i = 0; i < 9; i++)  // draw the nine visible lines of the chat box
		{
			sf::Text& text = _chatHistory[_lineFocus - (8 - i)]; //reference to current line to draw

			text.setPosition(_chatBox.getPosition().x + 10, (_chatBox.getPosition().y + 10) + (12.f * i)); //set position in draw as lines can be scrolled
			this->_data->window.draw(text);
		}
		
		this->_data->window.draw(_textInput);

		this->_data->window.display();
	}

	void GameState::InitGridPieces()
	{
		sf::Vector2u tempSize = this->_data->assets.GetTexture("X Piece").getSize();

		//set all piece positions based on the size of the piece images then turn the image invisible
		for (int x = 0; x < 3; x++)
		{
			for (int y = 0; y < 3; y++)
			{
				_gridPieces[x][y].setTexture(this->_data->assets.GetTexture("X Piece"));
				_gridPieces[x][y].setPosition(_gridSprite.getPosition().x + (tempSize.x * x) - 7, _gridSprite.getPosition().y + (tempSize.y * y) - 7);
				_gridPieces[x][y].setColor(sf::Color(255, 255, 255, 0));
			}
		}

	}

	void GameState::CheckAndPlacePiece(sf::Vector2i touchPoint)
	{
		//work out mouse click position relative to the playing grid
		sf::FloatRect gridSize = _gridSprite.getGlobalBounds();
		sf::Vector2f gapOutsideOfGrid = sf::Vector2f((SCREEN_WIDTH - gridSize.width) / 2, (SCREEN_HEIGHT - gridSize.height) / 2);
		sf::Vector2f gridLocalTouchPos = sf::Vector2f(touchPoint.x - gapOutsideOfGrid.x, touchPoint.y - gapOutsideOfGrid.y);

		sf::Vector2f gridSectionSize = sf::Vector2f(gridSize.width / 3, gridSize.height / 3);
		
		int column, row;

		// Check which column the user has clicked

		if (gridLocalTouchPos.x < gridSectionSize.x) // First Column

		{
			column = 1;
		}

		else if (gridLocalTouchPos.x < gridSectionSize.x * 2) // Second Column

		{
			column = 2;
		}

		else if (gridLocalTouchPos.x < gridSize.width) // Third Column

		{
			column = 3;
		}

		// Check which row the user has clicked

		if (gridLocalTouchPos.y < gridSectionSize.y) // First Row

		{
			row = 1;
		}

		else if (gridLocalTouchPos.y < gridSectionSize.y * 2) // Second Row

		{
			row = 2;
		}

		else if (gridLocalTouchPos.y < gridSize.height) // Third Row

		{
			row = 3;
		}


		//if the square is empty
		if (_gridArray[column - 1][row - 1] == EMPTY_PIECE)
		{
			//set square to the current turn and change turn then check if player has won
			_gridArray[column - 1][row - 1] = _turn;

			if (PLAYER_PIECE == _turn)
			{				
				_gameState = STATE_OTHER_PLAYING;
				_gridPieces[column - 1][row - 1].setTexture(this->_data->assets.GetTexture("X Piece"));
				this->CheckHasPlayerWon(_turn);

				_turn = OTHER_PIECE;
			}
			else if (OTHER_PIECE == _turn)
			{				
				_gameState = STATE_PLAYING;
				_gridPieces[column - 1][row - 1].setTexture(this->_data->assets.GetTexture("O Piece"));
				this->CheckHasPlayerWon(_turn);

				_turn = PLAYER_PIECE;
			}
			//set clicked square's piece visible
			_gridPieces[column - 1][row - 1].setColor(sf::Color(255, 255, 255, 255));

		}

	}

	void GameState::CheckHasPlayerWon(int player) // checks the various win combinations in a tic-tac-toe game 
	{
		//all possible victory combinations
		Check3PiecesForMatch(0, 0, 1, 0, 2, 0, player);
		Check3PiecesForMatch(0, 1, 1, 1, 2, 1, player);
		Check3PiecesForMatch(0, 2, 1, 2, 2, 2, player);
		Check3PiecesForMatch(0, 0, 0, 1, 0, 2, player);
		Check3PiecesForMatch(1, 0, 1, 1, 1, 2, player);
		Check3PiecesForMatch(2, 0, 2, 1, 2, 2, player);
		Check3PiecesForMatch(0, 0, 1, 1, 2, 2, player);
		Check3PiecesForMatch(0, 2, 1, 1, 2, 0, player);

		//for draws, check how many empty squares there are
		int emptyNum = 9;

		for (int x = 0; x < 3; x++)

		{

			for (int y = 0; y < 3; y++)
			{

				if (EMPTY_PIECE != _gridArray[x][y])

				{
					emptyNum--;
				}
			}
		}

		// check if the game is a draw

		if (0 == emptyNum && (STATE_WON != _gameState) && (STATE_LOSE != _gameState))
		{
			_gameState = STATE_DRAW;
		}

		// check if the game is over

		if (STATE_DRAW == _gameState || STATE_LOSE == _gameState || STATE_WON == _gameState)
		{
			this->_clock.restart();
		}
	}

	void GameState::Check3PiecesForMatch(int x1, int y1, int x2, int y2, int x3, int y3, int pieceToCheck) 
	{

		if (pieceToCheck == _gridArray[x1][y1] && pieceToCheck == _gridArray[x2][y2] && pieceToCheck == _gridArray[x3][y3])
		{
			std::string winningPieceStr;

			if (O_PIECE == pieceToCheck)
			{
				winningPieceStr = "O Win";
			}
			else
			{
				winningPieceStr = "X Win";
			}

			//replace winning pieces texture
			_gridPieces[x1][y1].setTexture(this->_data->assets.GetTexture(winningPieceStr));

			_gridPieces[x2][y2].setTexture(this->_data->assets.GetTexture(winningPieceStr));

			_gridPieces[x3][y3].setTexture(this->_data->assets.GetTexture(winningPieceStr));

			if (PLAYER_PIECE == pieceToCheck)

			{
				_gameState = STATE_WON;
			}

			else
			{
				_gameState = STATE_LOSE;
			}
		}

	}

	void GameState::PostMessage(std::string msg)   // posts a chat message
	{
		//pull all chat lines up by one
		for (auto i = 0; i < _chatHistory.size() - 1; i++)
		{
			_chatHistory[i].setString(_chatHistory[i + 1].getString());
		}

		_chatHistory.back().setString(msg);

		int lineSize = 0;
		int lastSpace = -1;



		// split overflowing text onto next line

		sf::String lastLine = _chatHistory.back().getString();

		for (auto i = 0; i < lastLine.getSize(); i++)
		{
			//work out size of line based on pixel size
			sf::Glyph glyph = _font.getGlyph(lastLine[i], _chatHistory[0].getCharacterSize(), false);

			if ((lineSize + glyph.advance) > 490) //checks maximum length of text allowed on one line
			{
				//if space has been pressed break line at last space
				if (lastSpace != -1)
				{
					//pull up lines again and add another new line containing overflowing text
					_chatHistory.back().setString(lastLine.toAnsiString().substr(0, lastSpace));
					for (auto i = 0; i < _chatHistory.size() - 1; i++)
					{
						_chatHistory[i].setString(_chatHistory[i + 1].getString());
					}
					_chatHistory.back().setString(lastLine.toAnsiString().substr(lastSpace + 1));
				}
				//otherwise break line at last suitable character
				else
				{
					_chatHistory.back().setString(lastLine.toAnsiString().substr(0, i));
					for (auto i = 0; i < _chatHistory.size() - 1; i++)
					{
						_chatHistory[i].setString(_chatHistory[i + 1].getString());
					}
					_chatHistory.back().setString(lastLine.toAnsiString().substr(i + 1));
				}

				//if line needed splitting reset variables but focus on new line
				lastLine = _chatHistory.back().getString();
				lastSpace = -1;
				i = 0;
				lineSize = 0;
			}
			else
				lineSize += glyph.advance;

			//if new line reset linesize
			if (lastLine[i] == '\n')
				lineSize = 0;
			//if space set lastspace
			if (lastLine[i] == ' ')
				lastSpace = i;
		}
		//set focus on newest line
		_lineFocus = _chatHistory.size() -1;
	}

	void GameState::UpdateInput()
	{
		//update input text to show realtime during typing
		int lineSize = 0;

		//goes backwards from end of string
		for (unsigned int i = _rawInput.length(); i > 0; i--)
		{
			sf::Glyph glyph = _font.getGlyph(_rawInput[i], _textInput.getCharacterSize(), false);

			if ((lineSize + glyph.advance) > 490) 
			{
				_textInput.setString(sf::String(_rawInput.substr(i)));
				return;
			}
			else
				lineSize += glyph.advance;
		}
		_textInput.setString(sf::String(_rawInput));
	}
}
