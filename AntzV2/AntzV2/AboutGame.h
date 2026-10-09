#pragma once
#include <windows.h>
#include "Renderer.h"
#include "Field.h"
#include "GameTypes.h"

class AboutGame
{
private:
	Renderer m_renderer;
	Field m_field;
	bool m_isRunning = false;
	std::vector<PlayerData> m_players;
	DialogState m_dialog;
	TextBox m_antBox;
	std::string m_error;
	GameSpeed m_gameSpeed = GameSpeed::Normal;
	int m_activePlayer = 1;

public:

	bool Initialize(HDC hdc)
	{
		m_renderer.Initialize(hdc);
		GameConfig config;
		config.m_DebugField = true;
		m_field.Initialize(config);

		m_players.resize(3);

		m_players[0].m_playerID = 0;
		m_players[0].m_playerName = "Unowned";

		m_players[1].m_playerID = 1;
		m_players[1].m_playerName = "Red";
		m_players[1].m_cells_owned = m_field.CountPlayerCells(1);

		m_players[2].m_playerID = 2;
		m_players[2].m_playerName = "Blue";
		m_players[2].m_cells_owned = m_field.CountPlayerCells(2);

		m_dialog.m_mode = DIALOG_GROWTH;

		return true;
	}

	void Start()
	{
		m_isRunning = true;
	}

	void Stop()
	{
		m_isRunning = false;
	}

	void Update()
	{
		if (!m_isRunning)
		{
			return;
		}

		// Walkthrough timing and phase changes will go here.
	}
	void Render() 
	{
		m_renderer.Render(m_field, m_players, m_dialog,
			m_activePlayer, m_antBox, m_error, m_gameSpeed);
	}
	bool IsRunning() const;

};
