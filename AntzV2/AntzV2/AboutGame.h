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

	static constexpr int MOUSE_TRAIL_COUNT = 4;
	POINT m_mousePos = { 100, 100 };
	POINT m_mouseTarget = { 100, 100 };
	POINT m_mouseTrail[MOUSE_TRAIL_COUNT] = {};
	bool m_mouseVisible = false;
	bool m_mouseMoving = false;
	int m_mouseTap = 0;

	struct TestMouseData
	{
		int row;
		int col;
	};

	TestMouseData m_testMouseData[5] =
	{
		{ 2, 2 },
		{ 2, 7 },
		{ 7, 7 },
		{ 7, 2 },
		{ 4, 4 }
	};

	int m_testMouseIndex = 0;
	int m_testMousePause = 0;

	void SetMouseTarget(int row, int col)
	{
		const auto& cells = m_renderer.GetCellRects();

		for (const auto& cell : cells)
		{
			if (cell.row == row && cell.col == col)
			{
				m_mouseTarget.x =
					(cell.rect.left + cell.rect.right) / 2;
				m_mouseTarget.y =
					(cell.rect.top + cell.rect.bottom) / 2;

				if (!m_mouseVisible)
				{
					m_mousePos = m_mouseTarget;

					for (int i = 0; i < MOUSE_TRAIL_COUNT; ++i)
					{
						m_mouseTrail[i] = m_mousePos;
					}

					m_mouseVisible = true;
				}

				m_mouseMoving = true;
				m_mouseTap = 0;
				return;
			}
		}
	}

	void Mouse()
	{
		if (!m_mouseVisible)
		{
			return;
		}

		m_mouseTrail[3] = m_mouseTrail[2];
		m_mouseTrail[2] = m_mouseTrail[1];
		m_mouseTrail[1] = m_mouseTrail[0];
		m_mouseTrail[0] = m_mousePos;

		if (m_mousePos.x != m_mouseTarget.x ||
			m_mousePos.y != m_mouseTarget.y)
		{
			int dx = m_mouseTarget.x - m_mousePos.x;
			int dy = m_mouseTarget.y - m_mousePos.y;

			if (dx > 12) dx = 12;
			if (dx < -12) dx = -12;
			if (dy > 12) dy = 12;
			if (dy < -12) dy = -12;

			m_mousePos.x += dx;
			m_mousePos.y += dy;

			m_mouseMoving =
				m_mousePos.x != m_mouseTarget.x ||
				m_mousePos.y != m_mouseTarget.y;
		}
		else if (m_mouseTap < 3)
		{
			++m_mouseTap;
		}
	}


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

		Mouse();

		if (!m_mouseVisible)
		{
			SetMouseTarget(
				m_testMouseData[0].row,
				m_testMouseData[0].col);

			m_testMouseIndex = 1;
			return;
		}

		if (m_mouseMoving || m_mouseTap < 3)
		{
			return;
		}

		if (m_testMouseIndex >= 5)
		{
			return;
		}

		SetMouseTarget(
			m_testMouseData[m_testMouseIndex].row,
			m_testMouseData[m_testMouseIndex].col);

		++m_testMouseIndex;
	}

	void Render() 
	{
		m_renderer.Render(m_field, m_players, m_dialog,
			m_activePlayer, m_antBox, m_error, m_gameSpeed);
		m_renderer.RenderMouse( m_mouseTrail, MOUSE_TRAIL_COUNT,
				m_mouseVisible, m_mouseTap);

	}
	bool IsRunning() const;

};
