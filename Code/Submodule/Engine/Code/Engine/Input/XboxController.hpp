#include "Engine/Input/AnalogStick.hpp"
#include "Engine/Input/KeyButton.hpp"

enum XboxButtonID
{
	XBOX_BUTTON_ID_INVALID = -1,

	// Physical buttons
	XBOX_BUTTON_ID_A,
	XBOX_BUTTON_ID_B,
	XBOX_BUTTON_ID_X,
	XBOX_BUTTON_ID_Y,

	XBOX_BUTTON_ID_BACK,
	XBOX_BUTTON_ID_START,

	XBOX_BUTTON_ID_LSHOULDER,
	XBOX_BUTTON_ID_RSHOULDER,

	XBOX_BUTTON_ID_LTHUMB,
	XBOX_BUTTON_ID_RTHUMB,

	XBOX_BUTTON_ID_DPAD_RIGHT,
	XBOX_BUTTON_ID_DPAD_UP,
	XBOX_BUTTON_ID_LEFT,
	XBOX_BUTTON_ID_DOWN,

	NUM_XBOX_BUTTONS
};

class XboxController
{

	friend class InputSystem;


public:
	explicit XboxController( int controllerID );
	~XboxController();

	bool               IsConnected()         const { return m_isConnected; }
	int                GetControllerID()     const { return m_controllerID; }
	const AnalogStick& GetLeftAnalogStick()  const { return m_leftAnalogStick; }
	const AnalogStick& GetRightAnalogStick() const { return m_rightAnalogStick; }
	float              GetLeftTrigger()      const { return m_leftTriggerValue; }
	float              GetRightTrigger()     const { return m_rightTriggerValue; }
	const KeyButton&   GetButtonState( XboxButtonID buttonID ) const;


private:
	void Update();
	void Reset();
	void UpdateTrigger( float& triggerValue, unsigned char rawValue );
	void UpdateAnalogStick( AnalogStick& analogStick, short rawX, short rawY );
	void UpdateButton( XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag );
	
	void ResetTrigger();
	void ResetAnalogStick();
	void ResetButton();

public:
	const int m_controllerID = -1;
	bool m_isConnected = false;
	KeyButton m_buttonStates[ NUM_XBOX_BUTTONS ];
	AnalogStick m_leftAnalogStick = AnalogStick( 0.3f, 0.9f );
	AnalogStick m_rightAnalogStick = AnalogStick( 0.3f, 0.9f );
	float m_leftTriggerValue = 0.0f;
	float m_rightTriggerValue = 0.0f;
};



