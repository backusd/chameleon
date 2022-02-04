#pragma once
#include "pch.h"




///////////////////////
// MY CLASS INCLUDES //
///////////////////////
/*
#include "d3dclass.h"
#include "inputclass.h"
#include "textureshaderclass.h"
#include "bitmapclass.h"
#include "textclass.h"
*/

class UserInterfaceClass
{
public:
	UserInterfaceClass();
	~UserInterfaceClass();


	void Shutdown();

	void CheckForChatMessage(char*, bool&);
	bool AddChatMessageFromServer(char*, int);
	/*
	bool Initialize(D3DClass*, HWND, int, int);
	bool Frame(D3DClass*, InputClass*, int, int, int);
	bool Render(D3DClass*, TextureShaderClass*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);

	bool UpdateDrawCount(int);
	
	*/

private:



	bool m_newMessage;
	char m_serverMessage[50];
	/*
	
	bool SetupTextStrings(D3DClass*);
	bool SetVideoCardInfo(ID3D10Device*, char*, int);
	bool SetFps(ID3D10Device*);
	bool UpdateFps(int);
	bool SetCpu(ID3D10Device*);
	bool UpdateCpu(int);
	bool SetLatency(ID3D10Device*);
	bool UpdateLatency(int);
	bool SetDrawCount(ID3D10Device*);
	bool SetChatTextStrings(ID3D10Device*);
	void KeyPressUpdate(int, ID3D10Device*);
	bool UpdateChatBar(ID3D10Device*);
	bool AddChatMessage();
	void AddChatMessageForServer();

	BitmapClass* m_ChatWindow;
	int m_chatWindowX, m_chatWindowY;
	TextClass* m_Text;
	int m_talkBarPosition;
	char m_chatBarString[50];
	char m_chatString1[50];
	char m_chatString2[50];
	char m_chatString3[50];
	char m_chatString4[50];
	char m_chatString5[50];
	char m_chatString6[50];
	char m_chatString7[50];
	char m_serverMessage[50];
	
	*/
};