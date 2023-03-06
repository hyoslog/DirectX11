#pragma once

//////////////////////////////////////////////////////////////////////////
// 클래스
//////////////////////////////////////////////////////////////////////////
// 키보드 입력 처리
class CInput final
{
public:
	CInput();
	CInput(const CInput& InOther);
	~CInput();

public:
	void Initialize();

public:
	void KeyDown(unsigned int InInput);
	void KeyUp(unsigned int InInput);

	bool IsKeyDown(unsigned int Inkey);

private:
	bool Keys[256];
};