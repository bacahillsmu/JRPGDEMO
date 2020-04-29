#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Memory/Memory.hpp"

#include <map>
#include <vector>
#include <mutex>

class RenderContext;
class BitMapFont;
struct Camera;
struct Rgba;

extern std::atomic<size_t> memtrack_allocationCount;
extern std::atomic<size_t> memtrack_byteCount;

class DevConsole
{
public:

	// Constructor/Deconstructor
	DevConsole();
	~DevConsole();

	// Game Flow
	void Startup();
	void BeginFrame();
	void Render( BitMapFont* fontToUse ) const;
	void EndFrame();
	void Shutdown();

	// Display self
	void Open();
	void Close();
	bool IsOpen() const;
	void OpenAtEndOfFrame();
	void CloseAtEndOfFrame();

	// Key Actions
	void Backspace();
	void Delete();
	void Enter();
	void Esc();
	
	// Drawing Dev Console Boxes
	void DrawDevConsoleUI() const;
	void CreateDevConsoleBackGround() const;
	void CreateDevConsoleTextAreaTyping() const;
	void DrawDevConsoleCursor() const;

	// Cursor
	void UpdateCursorPosition(int moveDirection);
	void UpdateCursorPositionWithShift(int moveDirection);
	void HighlightCheck();
	void DisplayHighlight() const;
	void ResetHighlight();

	// Display text
	void Print(const std::string& devConsolePrintString);
	void AddStringToTextOutput( const Rgba& textColor, const std::string& devConsolePrintString );
	void AddLetterToCurrentTypingText( const Rgba& textColor, const std::string& devConsolePrintString );
	void DisplayTextInTextArea(float lineHeight, BitMapFont* fontToUse) const;
	void DisplayTextInTypingArea(float lineHeight, BitMapFont* fontToUse) const;
	void ClearDevConsoleTypingArea();
	
	void MoveHistory(int moveDirection);
	void ExecuteCommandLine(const std::string& commandLine);
	void RemoveOldestText();
	void ResetValues();
	static bool Command_Test(EventArgs& args);

	// Gets
	int GetCurrentTypingAreaTextSize();

	bool HandleKeyPressed( unsigned char asKey );

	static std::string GetByteSizeString(size_t byte_count);

	std::mutex m_devConsoleLock;

private:

	bool isOpen							= false;
	bool m_willOpenAtEndOfFrame			= false;
	bool m_willCloseAtEndOfFrame		= false;

	unsigned int m_frameCount			= 0;
	int m_lastTextPrinted				= 0;
	int m_startCursorPositionShift		= -1;
	int m_cursorPosition				= 0;
	int m_historyPosition				= -1;

	float m_timeStamp					= 0.0f;
	float m_startTime					= 0.0f;
	float m_lineHeight					= 0.4f;

	std::string m_currentTypingText		= "";

	Camera* m_devConsoleCamera		= nullptr;
	
	Vec2 m_startHighlightPosition	= Vec2(0.0f, 0.0f);
	Vec2 m_endHighlightPosition		= Vec2(0.0f, 0.0f);
	
	std::vector<unsigned int>		m_printFrame;
	std::vector<float>				m_printTime;
	std::vector<std::string>		m_textCommands;
	std::vector<std::string>		m_textHistory;
	std::vector<std::string>		m_texts;
	std::vector<Rgba>				m_textcolors;
};

extern DevConsole*	g_theDevConsole;


