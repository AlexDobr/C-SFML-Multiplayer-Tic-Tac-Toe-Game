#include "stdafx.h"
#include "ConnectingState.h"
#include "GameState.h"
#include <iostream>
#include "DEFINITIONS.h"
#include <sstream>

namespace TTT
{

	
	//sets up connection between host and client

	ConnectingState::ConnectingState(GameDataRef data) :_data(data)
	{
	}

	void ConnectingState::Init()
	{
		//load textures

		this->_data->assets.LoadTexture("Main Menu Background", MAIN_MENU_BACKGROUND_FILEPATH);
		this->_data->assets.LoadTexture("Game Title", MAIN_MENU_TITLE_FILEPATH);
		this->_data->assets.LoadTexture("Host Button", HOST_BUTTON);
		this->_data->assets.LoadTexture("Join Button", JOIN_BUTTON);


		//set textures

		_background.setTexture(this->_data->assets.GetTexture("Main Menu Background"));
		_hostButton.setTexture(this->_data->assets.GetTexture("Host Button"));
		_joinButton.setTexture(this->_data->assets.GetTexture("Join Button"));


		//set positions

		// align to the top middle of the window
		this->_hostButton.setPosition((SCREEN_WIDTH / 2) - (this->_hostButton.getGlobalBounds().width / 2), (SCREEN_HEIGHT / 3) - (this->_hostButton.getGlobalBounds().height / 2));
		this->_joinButton.setPosition((SCREEN_WIDTH / 2) - (this->_joinButton.getGlobalBounds().width / 2), (SCREEN_HEIGHT / 3*2) - (this->_joinButton.getGlobalBounds().height / 2));



	}

	void ConnectingState::HandleInput()
	{
		sf::Event event;
		while (this->_data->window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				this->_data->window.close();
			}

			if (this->_data->input.IsSpriteClicked(_hostButton, sf::Mouse::Left, this->_data->window))
			{
				sf::TcpListener listener;
				if (listener.listen(53000) != sf::Socket::Done)
				{
				}
				else 
					std::cout << "Listening for client" << std::endl;
			

				if (listener.accept(_data->socket) != sf::Socket::Done)
				{
				}
				else
					std::cout << "Client received and established" << std::endl;
				

				listener.close();
				std::cout << "Listening stopped" << std::endl;

				this->_data->machine.AddState(StateRef(new GameState(_data, true)), true);
			}

			if (this->_data->input.IsSpriteClicked(_joinButton, sf::Mouse::Left, this->_data->window))
			{
				sf::IpAddress ip = sf::IpAddress::getLocalAddress();
				if (_data->socket.connect(ip, 53000) != sf::Socket::Done)
				{
				}
				else
				{
					std::cout << "Connected to host" << std::endl;
				}

				this->_data->machine.AddState(StateRef(new GameState(_data, false)), true);
			}
		}

	}

	void ConnectingState::Update(float dt)
	{
	}

	void ConnectingState::Draw(float dt)
	{
		this->_data->window.clear();

		this->_data->window.draw(_background);
		this->_data->window.draw(_hostButton);
		this->_data->window.draw(_joinButton);

		this->_data->window.display();
	}



}
