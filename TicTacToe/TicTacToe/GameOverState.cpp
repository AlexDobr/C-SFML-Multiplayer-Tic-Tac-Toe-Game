#include "stdafx.h"
#include "GameOverState.h"
#include "MainMenuState.h"
#include "GameState.h"
#include <iostream>
#include "DEFINITIONS.h"
#include <sstream>

namespace TTT
{
	GameOverState::GameOverState(GameDataRef data) :_data(data)
	{

	}

	void GameOverState::Init()
	{
		//load textures

		this->_data->assets.LoadTexture("Retry Button", RETRY_BUTTON);
		this->_data->assets.LoadTexture("Home Button", HOME_BUTTON);


		//set textures

		_retryButton.setTexture(this->_data->assets.GetTexture("Retry Button"));
		_homeButton.setTexture(this->_data->assets.GetTexture("Home Button"));


		//set positions

		this->_retryButton.setPosition((SCREEN_WIDTH / 2) - (this->_retryButton.getGlobalBounds().width / 2), (SCREEN_HEIGHT / 3) - (this->_retryButton.getGlobalBounds().height / 2));
		this->_homeButton.setPosition((SCREEN_WIDTH / 2) - (this->_homeButton.getGlobalBounds().width / 2), (SCREEN_HEIGHT / 3 * 2) - (this->_homeButton.getGlobalBounds().height / 2));


	}

	void GameOverState::HandleInput()
	{
		sf::Event event;
		while (this->_data->window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				this->_data->window.close();
			}

			if (this->_data->input.IsSpriteClicked(_retryButton, sf::Mouse::Left, this->_data->window))
			{
				//this->_data->machine.AddState(StateRef(new GameState(_data)), true);
			}

			if (this->_data->input.IsSpriteClicked(_homeButton, sf::Mouse::Left, this->_data->window))
			{
				
				this->_data->machine.AddState(StateRef(new MainMenuState(_data)), true);
			}

		}

	}

	void GameOverState::Update(float dt)
	{

	}

	void GameOverState::Draw(float dt)
	{
		this->_data->window.clear(sf::Color::Magenta);

		
		this->_data->window.draw(_retryButton);
		this->_data->window.draw(_homeButton);

		this->_data->window.display();
	}
}
