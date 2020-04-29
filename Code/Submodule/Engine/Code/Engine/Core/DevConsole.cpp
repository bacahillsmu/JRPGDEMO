#include "ThirdParty/RakNet/RakNetInterface.hpp"

#include "Engine/Core/DevConsole.hpp"
#include "Engine/Renderer/BitMapFont.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"
//#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Game/Framework/GameCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/EngineBuildPreferences.hpp"
#include "Engine/Log/Log.hpp"
#include "Game/Framework/App.hpp"



#include <vector>


DevConsole* g_theDevConsole = nullptr;

DevConsole::DevConsole()
{

}

DevConsole::~DevConsole()
{
	delete m_devConsoleCamera;
	m_devConsoleCamera = nullptr;
}

// static void LogMessageToDevConsole(log_message_t* msg)
// {
// 	std::scoped_lock lk(g_theDevConsole->m_devConsoleLock);
// 
// 	std::string message = Stringf("%s: %s", msg->filter, msg->message);
// 
// 	g_theDevConsole->AddStringToTextOutput(Rgba::YELLOW, message);
// }

void DevConsole::Startup()
{
	m_startTime = (float)GetCurrentTimeSeconds();

	m_devConsoleCamera = new Camera();
	m_devConsoleCamera->SetOrthographicProjection( Vec2( 0.0f, 0.0f ), Vec2( 20.0f, 10.0f ) );


	//g_theLogSystem->LogHook(LogMessageToDevConsole);
}

void DevConsole::BeginFrame()
{
	RemoveOldestText();
}

void DevConsole::EndFrame()
{
	m_frameCount++;
	m_timeStamp = (float)GetCurrentTimeSeconds() - m_startTime;

	if(m_willOpenAtEndOfFrame)
	{
		Open();
	}
	else if(m_willCloseAtEndOfFrame)
	{
		Close();
	}
}

void DevConsole::Shutdown()
{

}

void DevConsole::Print(const std::string& devConsolePrintString)
{
	AddStringToTextOutput(Rgba::WHITE, devConsolePrintString);
}

void DevConsole::AddStringToTextOutput( const Rgba& textColor, const std::string& devConsolePrintString )
{
	m_texts.push_back(devConsolePrintString);
	m_textcolors.push_back(textColor);
	m_printTime.push_back(m_timeStamp);
	m_printFrame.push_back(m_frameCount);
}

void DevConsole::AddLetterToCurrentTypingText( const Rgba& textColor, const std::string& devConsolePrintString )
{
	std::string firstChunk = m_currentTypingText.substr(0, m_cursorPosition);
	std::string secondChunk = m_currentTypingText.substr(m_cursorPosition, m_currentTypingText.size());
	m_currentTypingText = firstChunk + devConsolePrintString + secondChunk;

	m_textcolors.push_back(textColor);
}

void DevConsole::Render( BitMapFont* fontToUse ) const
{
	if(!isOpen)
	{
		return;
	}

	m_devConsoleCamera->SetModelMatrix(Matrix4x4::IDENTITY);
	g_theRenderer->BindModelMatrix(m_devConsoleCamera->m_cameraModel);
 	m_devConsoleCamera->SetColorTargetView(g_theRenderer->GetFrameColorTarget());

 	g_theRenderer->BeginCamera(m_devConsoleCamera);

	// Load Shader
	Shader* shader = g_theRenderer->GetOrCreateShader( "Data/Shaders/default_unlit_devconsole.shader" );
	g_theRenderer->BindShader(shader);

	// Draw the dev console UI
	DrawDevConsoleUI();	

	// Draw Cursor
	DrawDevConsoleCursor();

	// Display the text that is being written
	DisplayTextInTypingArea(m_lineHeight, fontToUse);		

	// Highlight
	DisplayHighlight();

	// Display the text that has been written
	DisplayTextInTextArea(m_lineHeight, fontToUse);		

	g_theRenderer->EndCamera();
}

void DevConsole::Close()
{
	isOpen = false;

	m_willOpenAtEndOfFrame = false;
	m_willCloseAtEndOfFrame = false;
}

void DevConsole::Open()
{
	isOpen = true;

	m_willOpenAtEndOfFrame = false;
	m_willCloseAtEndOfFrame = false;
}

void DevConsole::OpenAtEndOfFrame()
{
	m_willOpenAtEndOfFrame = true;
}

void DevConsole::CloseAtEndOfFrame()
{
	m_willCloseAtEndOfFrame = true;
}

bool DevConsole::IsOpen() const
{
	return isOpen;
}

void DevConsole::DrawDevConsoleUI() const
{
	g_theRenderer->BindTextureViewWithSampler(0, nullptr);

	CreateDevConsoleBackGround();
	CreateDevConsoleTextAreaTyping();		
}

void DevConsole::CreateDevConsoleBackGround() const
{
	std::vector<Vertex_PCU> backGroundQuad;

	AABB2 whiteBox = AABB2::MakeFromMinsMaxs( Vec2( 0.0f, 0.0f ), Vec2( 10.0f, 5.0f ) );
	AABB2 blackBox = AABB2::MakeFromMinsMaxs( Vec2( 0.05f, 0.05f ), Vec2( 9.9f, 4.9f ) );	

	AddVertsForAABB2D(backGroundQuad, whiteBox, Rgba::WHITE);
	AddVertsForAABB2D(backGroundQuad, blackBox, Rgba::BLACK);	

	g_theRenderer->DrawVertexArray( backGroundQuad );	
}

void DevConsole::CreateDevConsoleTextAreaTyping() const
{
	std::vector<Vertex_PCU> typingAreaQuad;

	AABB2 border = AABB2::MakeFromMinsMaxs( Vec2( 0.1f, 0.1f ), Vec2( 9.85f, 0.75f ) );
	AABB2 box = AABB2::MakeFromMinsMaxs( Vec2( 0.15f, 0.15f ), Vec2( 9.8f, 0.7f ) );	

	AddVertsForAABB2D(typingAreaQuad, border, Rgba::WHITE);
	AddVertsForAABB2D(typingAreaQuad, box, Rgba::BLACK);	

	g_theRenderer->DrawVertexArray( typingAreaQuad );	
}

void DevConsole::DrawDevConsoleCursor() const
{
	float displayHeight = 0.2f;
	displayHeight /= g_theApp->m_devConsoleFont->m_fontHeight;

	g_theRenderer->BindTextureViewWithSampler(0, nullptr);
	std::vector<Vertex_PCU> consoleCursorQuad;
	
	float textOffset = 0.0f;
	int numChars = (int)m_currentTypingText.length();
	for (int charIndex = 0; charIndex < numChars; ++charIndex)
	{
		if (charIndex == m_cursorPosition)
		{
			break;
		}

		int index = m_currentTypingText[charIndex];
		GlyphData glyph = g_theApp->m_devConsoleFont->m_glyphData[index];
		textOffset += displayHeight * glyph.m_cellHeightFractionBeforeGlyph;
		textOffset += displayHeight * glyph.m_cellHeightFractionDuringGlyph;
		textOffset += displayHeight * glyph.m_cellHeightFractionAfterGlyph;
	}

	Vec2 bottomLeft = Vec2( 0.152f + textOffset, 0.18f );
	Vec2 topRight = Vec2( 0.162f + textOffset, 0.65f );

	AABB2 box = AABB2::MakeFromMinsMaxs(bottomLeft, topRight);

	AddVertsForAABB2D(consoleCursorQuad, box, Rgba::WHITE);
	if((int)m_timeStamp % 2 == 0)
	{
		g_theRenderer->DrawVertexArray( consoleCursorQuad );
	}
}

void DevConsole::HighlightCheck()
{
	if(m_startCursorPositionShift != -1)
	{
		if(m_startCursorPositionShift < m_cursorPosition)
		{
			m_startHighlightPosition = Vec2(0.15f + (float)(m_startCursorPositionShift) * 0.1f, 0.15f);
			m_endHighlightPosition = Vec2(0.15f + (float)(m_cursorPosition) * 0.1f , 0.7f);
		}
		else if(m_cursorPosition < m_startCursorPositionShift)
		{
			m_startHighlightPosition = Vec2(0.15f + (float)(m_cursorPosition) * 0.1f, 0.18f);
			m_endHighlightPosition = Vec2(0.15f + (float)(m_startCursorPositionShift) * 0.1f, 0.7f);
		}
		else
		{
			m_startHighlightPosition = Vec2(0.0f, 0.0f);
			m_endHighlightPosition = Vec2(0.0f, 0.0f);
		}
	}
}

void DevConsole::DisplayHighlight() const
{
	if(m_startHighlightPosition != Vec2(0.0f, 0.0f) && m_endHighlightPosition != Vec2(0.0f, 0.0f))
	{
		AABB2 box = AABB2::MakeFromMinsMaxs(m_startHighlightPosition, m_endHighlightPosition);
		std::vector<Vertex_PCU> someQuad;
		Rgba boxColor = Rgba(1.0f, 1.0f, 1.0f, 0.5f);

		AddVertsForAABB2D(someQuad, box, boxColor);

		g_theRenderer->BindTextureViewWithSampler(0, nullptr);
		g_theRenderer->DrawVertexArray( someQuad );
	}	
}

void DevConsole::DisplayTextInTextArea(float lineHeight, BitMapFont* fontToUse) const
{
	float displayHeight = 0.2f;

	std::vector<Vertex_PCU> textVerts;
	Vec2 textStartPosition( 0.5f, 1.5f);

	for(int textIndex = 0; textIndex < m_texts.size(); textIndex++)
	{
		Vec2 printPosition(0.0f, (float)(m_texts.size() - textIndex));
		
		fontToUse->AddVertsForText2D(textVerts, (textStartPosition + printPosition) * lineHeight, displayHeight, m_texts[textIndex], m_textcolors[textIndex]);
	}

	if((int)textVerts.size() > 0)
	{
		g_theRenderer->BindTextureView( 0u, fontToUse->GetTextureView() );
		g_theRenderer->DrawVertexArray((int)textVerts.size(), &textVerts[0]);
	}
}

void DevConsole::DisplayTextInTypingArea(float lineHeight, BitMapFont* fontToUse) const
{
	UNUSED(lineHeight);
	float displayHeight = 0.2f;

	std::vector<Vertex_PCU> textVerts;
	Vec2 textStartPosition = Vec2( 0.15f, 0.15f );

	fontToUse->AddVertsForText2D(textVerts, textStartPosition, displayHeight, m_currentTypingText, Rgba::WHITE);

	if((int)textVerts.size() > 0)
	{
		g_theRenderer->BindTextureView( 0u, fontToUse->GetTextureView() );
		g_theRenderer->DrawVertexArray((int)textVerts.size(), &textVerts[0]);
	}
}

void DevConsole::UpdateCursorPosition(int moveDirection)
{
	if(m_startCursorPositionShift == -1)
	{
		m_cursorPosition += moveDirection;
		m_cursorPosition = Clamp(m_cursorPosition, 0, (int)m_currentTypingText.size());
	}
	else
	{
		if(m_cursorPosition < m_startCursorPositionShift && moveDirection == 1)
		{
			m_cursorPosition = m_startCursorPositionShift;
		}
		else if(m_cursorPosition > m_startCursorPositionShift && moveDirection == -1)
		{
			m_cursorPosition = m_startCursorPositionShift;
		}
	}
	
	g_theDevConsole->ResetValues();
}

void DevConsole::UpdateCursorPositionWithShift(int moveDirection)
{
	if(m_startCursorPositionShift == -1)
	{
		m_startCursorPositionShift = m_cursorPosition;
	}	

	m_cursorPosition += moveDirection * 2;
	m_cursorPosition = (int)Clamp((float)m_cursorPosition, (float)0, (float)(m_currentTypingText.size() * 2));

	HighlightCheck();
}

void DevConsole::Backspace()
{
	std::string firstChunk = m_currentTypingText.substr(0, m_cursorPosition);
	std::string secondChunk = m_currentTypingText.substr(m_cursorPosition, m_currentTypingText.size());

	if(m_startCursorPositionShift == -1)
	{
		firstChunk = firstChunk.substr(0, (m_cursorPosition) - 1);
		UpdateCursorPosition(-1);
	}
	else if(m_startCursorPositionShift != -1)
	{
		if(m_startCursorPositionShift < m_cursorPosition)
		{
			firstChunk = m_currentTypingText.substr(0, (m_startCursorPositionShift));
			secondChunk = m_currentTypingText.substr((m_cursorPosition), m_currentTypingText.size());
			m_cursorPosition = m_startCursorPositionShift;
		}
		else if(m_cursorPosition < m_startCursorPositionShift)
		{
			firstChunk = m_currentTypingText.substr(0, (m_cursorPosition));
			secondChunk = m_currentTypingText.substr((m_startCursorPositionShift), m_currentTypingText.size());
		}

		UpdateCursorPosition(0);
	}

	m_currentTypingText = firstChunk + secondChunk;	
}

void DevConsole::Delete()
{
	std::string firstChunk = m_currentTypingText.substr(0, m_cursorPosition);
	std::string secondChunk = m_currentTypingText.substr(m_cursorPosition, m_currentTypingText.size());

	if(m_startCursorPositionShift == -1)
	{
		if(secondChunk.size() != 0)
		{
			secondChunk = secondChunk.substr(1, m_currentTypingText.size());
		}	
	}
	else if(m_startCursorPositionShift != -1)
	{
		if(m_startCursorPositionShift < m_cursorPosition)
		{
			firstChunk = m_currentTypingText.substr(0, (m_startCursorPositionShift));
			secondChunk = m_currentTypingText.substr((m_cursorPosition), m_currentTypingText.size());
			m_cursorPosition = m_startCursorPositionShift;
		}
		else if(m_cursorPosition < m_startCursorPositionShift)
		{
			firstChunk = m_currentTypingText.substr(0, (m_cursorPosition));
			secondChunk = m_currentTypingText.substr((m_startCursorPositionShift), m_currentTypingText.size());
		}

		UpdateCursorPosition(0);
	}
	

	m_currentTypingText = firstChunk + secondChunk;
}

void DevConsole::Enter()
{
	ResetHighlight();
	ResetValues();

	if(m_currentTypingText.size() != 0)
	{
		g_theDevConsole->AddStringToTextOutput(Rgba::WHITE, m_currentTypingText);


		for(int i = 0; i < m_textHistory.size(); i++)
		{
			if(m_textHistory[i] == m_currentTypingText)
			{
				m_textHistory.erase(m_textHistory.begin() + i);
				break;
			}
		}

		m_textHistory.insert(m_textHistory.begin(), m_currentTypingText);

		ExecuteCommandLine( m_currentTypingText );

		m_historyPosition = -1;
		m_currentTypingText = "";
		m_cursorPosition = 0;		
	}	
}

void DevConsole::Esc()
{
	ClearDevConsoleTypingArea();
}

void DevConsole::ClearDevConsoleTypingArea()
{
	m_currentTypingText = "";
	m_cursorPosition = 0;
}

void DevConsole::MoveHistory( int moveDirection )
{
	ResetHighlight();

	m_historyPosition += moveDirection;
	if(m_historyPosition < 0)
	{
		m_currentTypingText = "";
		m_cursorPosition = 0;
		ResetValues();
	}
	else
	{
		if(m_textHistory.size() != 0)
		{
			m_historyPosition = (int)Clamp((float)m_historyPosition, 0.0f, (float)m_textHistory.size() - 1.0f);
			m_currentTypingText = m_textHistory[m_historyPosition];
			m_cursorPosition = (int)m_currentTypingText.size() * 2;
		}
		else
		{
			m_historyPosition -= 1;
		}
	}	
}

void DevConsole::ResetValues()
{
	// History tracker reset
	m_historyPosition = -1;

	// Highlight tracker reset
	m_startCursorPositionShift = -1;
	m_startHighlightPosition = Vec2(0.0f, 0.0f);
	m_endHighlightPosition = Vec2(0.0f, 0.0f);
}

void DevConsole::ResetHighlight()
{
	if(m_startCursorPositionShift != -1)
	{
		m_cursorPosition = m_startCursorPositionShift;
	}
	m_startCursorPositionShift = -1;
	m_startHighlightPosition = Vec2(0.0f, 0.0f);
	m_endHighlightPosition = Vec2(0.0f, 0.0f);
}

void DevConsole::RemoveOldestText()
{
	// Check size of DevConsole
	if(m_texts.size() > 10)
	{
		m_texts.erase(m_texts.begin());
		m_textcolors.erase(m_textcolors.begin());
		m_printTime.erase(m_printTime.begin());
		m_printFrame.erase(m_printFrame.begin());
	}
}

void DevConsole::ExecuteCommandLine( const std::string& commandLine )
{
	//Split the string to sensible key value pairs
	std::vector<std::string> splitStrings = SplitStringOnDelimiter(commandLine, ' ');

	g_theDevConsole->AddStringToTextOutput(Rgba::YELLOW, "> " + commandLine);

	EventArgs args;

	for(int stringIndex = 1; stringIndex < static_cast<int>(splitStrings.size()); stringIndex++)
	{
		//split on =
		std::vector<std::string> keyValSplit = SplitStringOnDelimiter(splitStrings[stringIndex], '=');
		if(keyValSplit.size() != 2)
		{
			g_theDevConsole->AddStringToTextOutput(Rgba::RED ," Args not read. Not correct number of args.");
			g_theDevConsole->AddStringToTextOutput(Rgba(1.0f, 0.5f, 0.3f, 1.0f), " Args should be two values split by =");
		}
		else
		{
			//Print the data we read
			//stringToPrint = "-> " + KeyValSplit[0] + " = " + KeyValSplit[1];
			//g_theDevConsole->AddStringToTextOutput(Rgba::YELLOW, stringToPrint);
			args.SetValue(keyValSplit[0], keyValSplit[1]);
		}
	}

	g_theEventSystem->FireEvent(splitStrings[0], args);
}

bool DevConsole::Command_Test( EventArgs& args )
{
	UNUSED( args );
	g_theDevConsole->AddStringToTextOutput( Rgba(1.0f, 0.66f, 0.0f, 1.0f), Stringf( "Test command received" ) );
	return false; // Does not consume event; continue to call other subscribers’ callback functions

}

int DevConsole::GetCurrentTypingAreaTextSize()
{
	return (int)m_currentTypingText.size();
}

// -----------------------------------------------------------------------
bool DevConsole::HandleKeyPressed( unsigned char asKey )
{
	// Handling the Virtual Key press...
	bool virtualKeyHandled = false;

	// ...if the Virtual Key is the right arrow;
	if(asKey == VirtualKey::RIGHTARROW)
	{
		if(InputSystem::IsShiftDown())
		{
			g_theDevConsole->UpdateCursorPositionWithShift(1);
		}
		else
		{
			g_theDevConsole->UpdateCursorPosition(1);			
		}

		virtualKeyHandled = true;
	}

	// ...if the Virtual Key is the left arrow;
	else if(asKey == VirtualKey::LEFTARROW)
	{
		if(InputSystem::IsShiftDown())
		{
			g_theDevConsole->UpdateCursorPositionWithShift(-1);
		}
		else
		{
			g_theDevConsole->UpdateCursorPosition(-1);			
		}

		virtualKeyHandled = true;
	}

	// ...if the Virtual Key is the up arrow;
	else if(asKey == VirtualKey::UPARROW)
	{
		g_theDevConsole->MoveHistory(1);

		virtualKeyHandled = true;
	}

	// ...if the Virtual Key is the down arrow;
	else if(asKey == VirtualKey::DOWNARROW)
	{
		g_theDevConsole->MoveHistory(-1);

		virtualKeyHandled = true;
	}

	// ...if the Virtual Key is the backspace key;
	else if(asKey == VirtualKey::BACKSPACE)
	{
		g_theDevConsole->Backspace();
		g_theDevConsole->ResetValues();

		virtualKeyHandled = true;
	}

	// ...if the Virtual Key is the delete key;
	else if(asKey == VirtualKey::DEL)
	{
		g_theDevConsole->Delete();
		g_theDevConsole->ResetValues();

		virtualKeyHandled = true;
	}

	// ...if the Virtual Key is the enter key;
	else if(asKey == VirtualKey::ENTER)
	{
		g_theDevConsole->Enter();

		virtualKeyHandled = true;
	}

	// ...if the Virtual Key is the escape key;
	else if(asKey == VirtualKey::ESC)
	{
		if(g_theDevConsole->GetCurrentTypingAreaTextSize() == 0)
		{
			CloseAtEndOfFrame();
		}
		else
		{
			g_theDevConsole->Esc();
			g_theDevConsole->ResetValues();
		}

		virtualKeyHandled = true;
	}

	return virtualKeyHandled;
}

std::string DevConsole::GetByteSizeString(size_t byte_count)
{
	uint numBytes = (uint)byte_count;
	std::string returnString = "";
	if (numBytes < 1024)
	{
		returnString += Stringf("%u B", numBytes);
	}
	else if (numBytes > 1024 && numBytes < 1024 * 1024)
	{
		returnString += Stringf("%.3f KiB", (float)numBytes / 1024);
	}
	else if (numBytes > 1024 * 1024 && numBytes < 1024 * 1024 * 1024)
	{
		returnString += Stringf("%.3f MiB", (float)numBytes / (1024 * 1024));
	}
	else if (numBytes > (1024 * 1024 * 1024))
	{
		returnString += Stringf("%.3f GiB", (float)numBytes / (1024 * 1024 * 1024));
	}

	return returnString;
}
