#include "stdafx.h"
#include "MainMenuState.h"
#include "ConnectingState.h"
#include <iostream>
#include "DEFINITIONS.h"
#include <sstream>

namespace TTT
{
	//used as a connection to other game states. Makes transition to Game State and is transitioned into from Game Over state.

	MainMenuState::MainMenuState(GameDataRef data):_data(data)
	{

	}

	void MainMenuState::Init()
	{
		//load textures

		this->_data->assets.LoadTexture("Main Menu Background", MAIN_MENU_BACKGROUND_FILEPATH);
		this->_data->assets.LoadTexture("Game Title", MAIN_MENU_TITLE_FILEPATH);
		this->_data->assets.LoadTexture("Play Button", PLAY_BUTTON);
		this->_data->assets.LoadTexture("Play Button Outer", PLAY_BUTTON_OUTER);
		
		
		//set textures
		
		_background.setTexture(this->_data->assets.GetTexture("Main Menu Background"));
		_title.setTexture(this->_data->assets.GetTexture("Game Title"));
		_playButton.setTexture(this->_data->assets.GetTexture("Play Button"));
		_playButtonOuter.setTexture(this->_data->assets.GetTexture("Play Button Outer"));


		//set positions

		// align to the top middle of the window
		this->_title.setPosition((SCREEN_WIDTH / 2) - (this->_title.getGlobalBounds().width / 2), this->_title.getGlobalBounds().height * 0.1);  
		this->_playButton.setPosition((SCREEN_WIDTH / 2) - (this->_playButton.getGlobalBounds().width / 2), (SCREEN_HEIGHT / 2) - (this->_playButton.getGlobalBounds().height / 2));
		this->_playButtonOuter.setPosition((SCREEN_WIDTH / 2) - (this->_playButtonOuter.getGlobalBounds().width / 2), (SCREEN_HEIGHT / 2) - (this->_playButtonOuter.getGlobalBounds().height / 2));


	}

	void MainMenuState::HandleInput()
	{
		sf::Event event;
		while (this->_data->window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				this->_data->window.close();
			}

			if (this->_data->input.IsSpriteClicked(_playButton, sf::Mouse::Left, this->_data->window))
			{
				this->_data->machine.AddState(StateRef(new ConnectingState(_data)), true);
			}


		}

	}

	void MainMenuState::Update(float dt)
	{

	}

	void MainMenuState::Draw(float dt)
	{
		this->_data->window.clear();

		this->_data->window.draw(_background);
		this->_data->window.draw(_title);
		this->_data->window.draw(_playButton);
		this->_data->window.draw(_playButtonOuter);

		this->_data->window.display();
	}



}
