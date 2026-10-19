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

	HDC m_hdc = nullptr;
	bool m_showDialog = true;
	bool m_nextRequested = false;
	RECT m_beginRect = {};
	RECT m_nextRect = {};

	static constexpr int MOUSE_TRAIL_COUNT = 4;
	POINT m_mousePos = { 100, 100 };
	POINT m_mouseTarget = { 100, 100 };
	POINT m_mouseTrail[MOUSE_TRAIL_COUNT] = {};
	bool m_mouseVisible = false;
	bool m_mouseMoving = false;
	bool m_showAntOverlay = false;
	int m_mouseTap = 0;
	int m_dialogWidth = 500;
	int m_dialogHeight = 120;
	int m_dialogOffsetX = 0;
	int m_dialogOffsetY = 35;
	int m_dialogIndex = 0;
	int m_delayTicks = 0;

	bool Delay(int ticks)
	{
		if (++m_delayTicks < ticks)
		{
			return false;
		}

		m_delayTicks = 0;
		return true;
	}


	enum class WalkState
	{
		INTRO,
		GROWTH,
		ATTACK,
		MOVE,
		TURN_TRANSITION,
		COMPLETE
	};

	WalkState m_state = WalkState::INTRO;
	int m_stateStep = 0;

	//void AdvanceScript()
	//{
	//	if (m_scriptStep + 1 >= 3)
	//	{
	//		return;
	//	}

	//	++m_scriptStep;

	//	const ScriptStep& step = m_script[m_scriptStep];

	//	m_showDialog = false;

	//	m_dialogIndex = step.dialogIndex;
	//	m_field.SetPlayerVisualMode(1);

	//	if (step.row >= 0 && step.col >= 0)
	//	{
	//		SetMouseTarget(step.row, step.col);
	//	}

	//}

	struct DialogData
	{
		std::vector<const wchar_t*> text;
	};
	std::vector<DialogData> m_DialogData =
	{
		{
			{
				L"Red is Player 1 and is a human player.", L"Blue is the opponent.",
				L"The cursor will demonstrate Red's actions."
			}
		},
		{
			{
				L"During Red's turn, the player must distribute all available ants.",
				L"Choose cells based on opponent's position and the player's",
				L"personal strategy.  Select this cell and add all available ants"
			}
		}

	};

	void RenderNextButton()
	{
		m_nextRect = { 935, 625, 1060, 670 };

		HBRUSH brush = CreateSolidBrush(RGB(60, 60, 60));
		FillRect(m_hdc, &m_nextRect, brush);
		DeleteObject(brush);

		FrameRect(m_hdc, &m_nextRect,
			static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));

		SetBkMode(m_hdc, TRANSPARENT);
		SetTextColor(m_hdc, RGB(255, 255, 255));

		DrawTextW(m_hdc, L"Next", -1, &m_nextRect,
			DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

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

	void SetMouseTarget(const RECT& rect)
	{
		m_mouseTarget.x = (rect.left + rect.right) / 2;
		m_mouseTarget.y = (rect.top + rect.bottom) / 2;

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
		else
		{
			m_mouseMoving = false;

			if (m_mouseTap < 3)
			{
				++m_mouseTap;
			}
		}
	}


	void RenderDialog()
	{
		if (m_DialogData.empty()) return;

		RECT dialogRect = {};
		dialogRect.left = m_mousePos.x + m_dialogOffsetX;
		dialogRect.top = m_mousePos.y + m_dialogOffsetY;
		dialogRect.right = dialogRect.left + m_dialogWidth;
		dialogRect.bottom = dialogRect.top + m_dialogHeight;

		HBRUSH brush = CreateSolidBrush(RGB(35, 35, 35));
		FillRect(m_hdc, &dialogRect, brush);
		DeleteObject(brush);

		HPEN pen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
		HGDIOBJ oldPen = SelectObject(m_hdc, pen);
		HGDIOBJ oldBrush = SelectObject(
			m_hdc, GetStockObject(HOLLOW_BRUSH));

		Rectangle(m_hdc, dialogRect.left, dialogRect.top,
			dialogRect.right, dialogRect.bottom);

		SelectObject(m_hdc, oldBrush);
		SelectObject(m_hdc, oldPen);
		DeleteObject(pen);

		SetBkMode(m_hdc, TRANSPARENT);
		SetTextColor(m_hdc, RGB(230, 210, 135));

		RECT textRect = dialogRect;
		InflateRect(&textRect, -20, -20);

		for (const wchar_t* line : m_DialogData[m_dialogIndex].text)
		{
			DrawTextW(m_hdc, line, -1, &textRect,
				DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);

			textRect.top += 28;
		}
	}
	void RenderAntOverlay(int ants)
	{
		RECT rect = m_renderer.GetAnt();

		// Highlight the ant-entry box.
		HPEN pen = CreatePen(PS_SOLID, 2, RGB(255, 230, 40));
		HGDIOBJ oldPen = SelectObject(m_hdc, pen);
		HGDIOBJ oldBrush =
			SelectObject(m_hdc, GetStockObject(HOLLOW_BRUSH));

		Rectangle(m_hdc, rect.left, rect.top,
			rect.right, rect.bottom);

		SelectObject(m_hdc, oldBrush);
		SelectObject(m_hdc, oldPen);
		DeleteObject(pen);

		// Display the scripted ant count.
		SetBkMode(m_hdc, TRANSPARENT);
		SetTextColor(m_hdc, RGB(0, 0, 0));

		std::string text = std::to_string(ants);

		DrawTextA(m_hdc, text.c_str(), -1, &rect,
			DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

public:

	bool Initialize(HDC hdc)
	{
		m_hdc = hdc;
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
		if (m_nextRequested)
		{
			m_nextRequested = false;

			if (m_showDialog)
			{
				m_showDialog = false;
			}
		}

		switch (m_state)
		{
		case WalkState::INTRO:
			if (!m_showDialog)
			{
				m_state = WalkState::GROWTH;
				m_stateStep = 0;
			}
			return;

		case WalkState::GROWTH:
			if (m_stateStep == 0)
			{
				m_players[1].sant =
					1 + m_players[1].m_cells_owned / 3;
				m_players[1].tant = 0;
				m_players[1].selectS = true;
				m_players[1].selectT = false;

				m_field.SetPlayerVisualMode(1);
				SetMouseTarget(2, 2);
				m_stateStep = 1;
				return;
			}

			if (m_stateStep == 1)
			{
				if (!m_mouseMoving && m_mouseTap >= 3)
				{
					SetMouseTarget(9, 4);
					m_stateStep = 2;
				}
				return;
			}

			if (m_stateStep == 2)
			{
				if (!m_mouseMoving && m_mouseTap >= 3)
				{

					m_dialogIndex = 1;
					m_showDialog = true;
					m_stateStep = 3;
				}
				return;
			}
			if (m_stateStep == 3)
			{
				if (Delay(4))
				{
					m_field.SetVisualMode(9, 4, Field::V_GROWTH);
					SetMouseTarget(m_renderer.GetAnt());
					m_stateStep = 4;
				}
				return;
			}
			if (m_stateStep == 4)
			{
				if (!m_mouseMoving && m_mouseTap >= 3)
				{
					m_showAntOverlay = true;
					m_stateStep = 5;
				}
				return;
			}
			if (m_stateStep == 5)
			{

				if (m_nextRequested)
				{
					m_nextRequested = false;

					m_players[m_activePlayer].tant =
						m_players[m_activePlayer].sant;

					m_players[m_activePlayer].selectS = true;
					m_players[m_activePlayer].selectT = true;

					m_showAntOverlay = false;
					m_stateStep = 6;
				}
				return;
			}
			return;

		case WalkState::ATTACK:
		case WalkState::MOVE:
		case WalkState::TURN_TRANSITION:
		case WalkState::COMPLETE:
			return;
		}
	}


	bool HandleMouse(UINT message, LPARAM lParam)
	{
		if (message != WM_LBUTTONDOWN) return false;

		POINT point = {
			static_cast<short>(LOWORD(lParam)),
			static_cast<short>(HIWORD(lParam))
		};

		if (m_showDialog && PtInRect(&m_nextRect, point))
		{
			m_nextRequested = true;
			return true;
		}

		return false;
	}

	void Render() 
	{
		m_renderer.Render(m_field, m_players, m_dialog,
			m_activePlayer, m_antBox, m_error, m_gameSpeed);
		if (m_showAntOverlay)
		{
			RenderAntOverlay(m_players[m_activePlayer].sant);
		}
		m_renderer.RenderMouse( m_mouseTrail, MOUSE_TRAIL_COUNT,
				m_mouseVisible, m_mouseTap);
		RenderNextButton();
		if (m_showDialog)
		{
			RenderDialog();
		}

	}
	bool IsRunning() const;

};
