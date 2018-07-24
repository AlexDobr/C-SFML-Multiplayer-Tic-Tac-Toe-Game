#include "stdafx.h"
#include "PauseState.h"
#include "MainMenuState.h"
#include "GameState.h"
#include <iostream>
#include "DEFINITIONS.h"
#include <sstream>

namespace TTT
{
	PauseState::PauseState(GameDataRef data) :_data(data)
	{

	}

	void PauseState::Init()
	{
		//load textures

		this->_data->assets.LoadTexture("Pause Background", PAUSE_BACKGROUND_FILEPATH);
		this->_data->assets.LoadTexture("Resume Button", RESUME_BUTTON);
		this->_data->assets.LoadTexture("Home Button", HOME_BUTTON);


		//set textures

		_background.setTexture(this->_data->assets.GetTexture("Pause Background"));
		_resumeButton.setTexture(this->_data->assets.GetTexture("Resume Button"));
		_homeButton.setTexture(this->_data->assets.GetTexture("Home Button"));


		//set positions

		this->_resumeButton.setPosition((SCREEN_WIDTH / 2) - (this->_resumeButton.getGlobalBounds().width / 2), (SCREEN_HEIGHT / 3) - (this->_resumeButton.getGlobalBounds().height / 2));
		this->_homeButton.setPosition((SCREEN_WIDTH / 2) - (this->_homeButton.getGlobalBounds().width / 2), (SCREEN_HEIGHT / 3*2) - (this->_homeButton.getGlobalBounds().height / 2));


	}

	void PauseState::HandleInput()
	{
		sf::Event event;
		while (this->_data->window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				this->_data->window.close();
			}

			if (this->_data->input.IsSpriteClicked(_resumeButton, sf::Mouse::Left, this->_data->window))
			{
				this->_data->machine.RemoveState();
			}

			if (this->_data->input.IsSpriteClicked(_homeButton, sf::Mouse::Left, this->_data->window))
			{
				this->_data->machine.RemoveState();
				this->_data->machine.AddState(StateRef(new MainMenuState(_data)), true);
			}

		}

	}

	void PauseState::Update(float dt)
	{

	}

	void PauseState::Draw(float dt)
	{
		this->_data->window.clear();

		this->_data->window.draw(_background);
		this->_data->window.draw(_resumeButton);
		this->_data->window.draw(_homeButton);

		this->_data->window.display();
	}
}
