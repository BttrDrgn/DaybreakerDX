#include "main.hpp"
#include "memory.hpp"

auto config_disableHDMode = false;
auto config_disableFSAA = false;

typedef HWND(__stdcall* create_window_ex_a_t)(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
create_window_ex_a_t origCreateWindowExA = NULL;
HWND __stdcall hookCreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	lpWindowName = "Higurashi Daybreak Kai";
	dwStyle = 0x00CF0000;
	nWidth = 1920;
	nHeight = 1080;

	return origCreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

typedef BOOL(__stdcall* set_current_directory_a_t)(LPCSTR lpPathName);
set_current_directory_a_t origSetCurrentDirectoryA = NULL;
BOOL __stdcall hookSetCurrentDirectoryA(LPCSTR lpPathName)
{
	//Allows running the game from a different binary directory, useful for debugging
	return false;
}


DWORD jmpBack_WMCharHack_BackSpace = 0x006614F0;
DWORD jmpBack_WMCharHack_Fail = 0x00661516;
DWORD call_WMCharHack_Input = 0x00660D80;
DWORD vk = 0;
char txt[4] = "";
__declspec(naked) void ASM_WMCharHack()
{
	__asm
	{
		mov eax, [ebp + 16] //wParam
		mov dword ptr[ebp - 28], eax
		mov dword ptr[vk], eax
	}
	__asm pushad 
	txt[0] = (char)(int)vk;
	__asm popad 
	__asm
	{
		cmp dword ptr[ebp - 28], VK_BACK
		je char_ret
		cmp dword ptr[ebp - 28], VK_RETURN
		je char_ret
		mov ecx,0x006D7A08
		call [call_WMCharHack_Input]
		jmp [jmpBack_WMCharHack_Fail]
		char_ret:
		jmp [jmpBack_WMCharHack_BackSpace]
	}
}

DWORD jmpBack_CharInputHack = 0x00660E3D;
__declspec(naked) void ASM_CharInputHack()
{
	__asm
	{
		push offset txt
		jmp [jmpBack_CharInputHack]
	}
}

void __cdecl init(int argc, char* argv[])
{
	SetProcessAffinityMask(GetCurrentProcess(), 1);

	MH_Initialize();
	MH_CreateHook(&CreateWindowExA, &hookCreateWindowExA, reinterpret_cast<LPVOID*>(&origCreateWindowExA));
	MH_CreateHook(&SetCurrentDirectoryA, &hookSetCurrentDirectoryA, reinterpret_cast<LPVOID*>(&origSetCurrentDirectoryA));
	MH_EnableHook(MH_ALL_HOOKS);

	//US Keyboard Input Hack
	//TODO: Modernize the code to actual C++ Implmentation, not assembly hacks
	replace(0x00660E38, (DWORD)ASM_CharInputHack);
	replace(0x006614E2, (DWORD)ASM_WMCharHack);

	//HD Mode
	if (!config_disableHDMode)
	{
		//Render Window
		//800x600 -> 1920x1080
		write(0x00639747, { 0x68, 0x38, 0x04, 0x00, 0x00, 0x68, 0x80, 0x07, 0x00, 0x00, 0x6A, 0x00, 0x6A, 0x00 });
		write(0x0063978D, { 0x68, 0x38, 0x04, 0x00, 0x00, 0x68, 0x80, 0x07, 0x00, 0x00, 0x6A, 0x00, 0x6A, 0x00 });
		write(0x006397F9, { 0x68, 0x38, 0x04, 0x00, 0x00, 0x68, 0x80, 0x07, 0x00, 0x00, 0x6A, 0x00, 0x6A, 0x00 });
		static float resX = 1920 / 2;
		static float resY = 1080 / 2;
		set(0x00643342 + 2, &resX);
		set(0x00643348 + 2, &resX);
		set(0x00643732 + 2, &resX);
		set(0x00643C2F + 2, &resX);
		set(0x00643C35 + 2, &resX);
		set(0x00643F31 + 2, &resX);
		set(0x00643F37 + 2, &resX);
		set(0x0063A547 + 2, &resX);
		set(0x00644782 + 2, &resX);
		set(0x006447B0 + 2, &resX);
		set(0x006336BA + 2, &resX);
		set(0x006336C7 + 2, &resX);
		set(0x006336C7 + 2, &resX);
		set(0x0064376B + 2, &resX);

		set(0x006508BB + 2, &resY);
		set(0x005CAE9F + 2, &resY);
		set(0x0044F26D + 2, &resY);
		set(0x0053B41D + 2, &resY);
		set(0x005C2B4D + 2, &resY);
		set(0x0064335F + 2, &resY);
		set(0x00643365 + 2, &resY);
		set(0x006437C7 + 2, &resY);
		set(0x00643C4C + 2, &resY);
		set(0x00643C52 + 2, &resY);
		set(0x00643F4E + 2, &resY);
		set(0x00643F54 + 2, &resY);
		set(0x0063A537 + 2, &resY);
		set(0x00643791 + 2, &resY);

		//Movie Resize
		//1.25 -> 2.00
		write(0x00404F64, { 0x68, 0x00, 0x00, 0x00, 0x40 });
		write(0x004055BF, { 0x68, 0x00, 0x00, 0x00, 0x40 });
		write(0x0040533E, { 0x68, 0x00, 0x00, 0x00, 0x40 });
		write(0x0040DFEC, { 0x68, 0x00, 0x00, 0x00, 0x40 });


		float hpX = 970;
		float hpY = 1600;
		//HP Bar
		set(0x6A4CD8, hpX);
		set(0x6A4CD4, hpY);
		//Power Bar &  CharacterName 
		float pwbX = 898;
		float chnX = 998;
		set(0x6A4CD0, pwbX);
		set(0x6A4CC8, chnX);

		//Charge Bar
		float chbX = 917;
		float chbY = 1690;
		float chtX = 886;
		float chtY = 1772;
		float chb2Y = 925;
		set(0x6A4CC0, chbX);
		set(0x6A4CAC, chbY);
		set(0x6A4C98, chb2Y);
		set(0x6A4CA8, chtX);
		set(0x6A4CA4, chtY);

		//Main Bar
		float mabY = 1720;
		float mabX = 866;
		float matX = 834;
		float matY = 1830;
		set(0x6A4CB0,mabY);
		set(0x6A4CB4,mabX);
		set(0x6A4CA0,matX);
		set(0x6A4C9C,matY);

		//AI Icon
		float aiX = 920;
		set(0x6A4CBC, aiX);

		//Red Balloon
		float rbY = 1825;
		float rbtlY = 1810;
		float rbtpY = 1850;
		float rbtp2Y = 1878;
		set(0x6A4C8C, rbY);
		set(0x6A1C80, rbtlY);
		set(0x6A4C88, rbtpY);
		set(0x6A4C84, rbtp2Y);

		//small fix for vs screen
		static float vschar_right = 700;
		set(0x00412004 + 2, &vschar_right);

		//FPS Counter
		//Doesn't work. For some reason the image gets cut off if it's > 800x600
		// PUSH  44480000 (800) -> PUSH 44f00000 (1920)
		//write(0x0063A03B,"\x68\x00\x00\xF0\x44,5);

		//Pause Icon
		//Doesn't work. For some reason the image gets cut off if it's > 800x600
		//write(0x0063A0A5,"\x68\x00\x00\x07\x44\x68\x00\x00\x70\x00",10);

		//4 Player HD Support
		write(0x00639A01, { 0x68,0x1C,0x02,0x00,0x00,0x68,0xc0,0x03,0x00,0x00,0x6a,0x00,0x6a,0x00 }); //Player 1 Top Left
		write(0x00639A19, { 0x68,0x38,0x04,0x00,0x00,0x68,0xc0,0x03,0x00,0x00,0x68,0x1c,0x02,0x00,0x00,0x6A,0x00 }); //Player 2 Top Right
		write(0x00639A34, { 0x68,0x1c,0x02,0x00,0x00,0x68,0x80,0x07,0x00,0x00,0x6A,0x00,0x68,0xc0,0x03,0x00,0x00 }); //Player 3 Bottom Left
		write(0x00639A4F, { 0x68,0x38,0x04,0x00,0x00,0x68,0x80,0x07,0x00,0x00,0x68,0x1c,0x02,0x00,0x00,0x68,0xc0,0x03,0x00,0x00 }); //Player 4 Bottom Right
		write(0x0063995B, { 0x68,0x1C,0x02,0x00,0x00,0x68,0xc0,0x03,0x00,0x00,0x6a,0x00,0x6a,0x00 }); //Player 1 Top Left
		write(0x00639973, { 0x68,0x38,0x04,0x00,0x00,0x68,0xc0,0x03,0x00,0x00,0x68,0x1c,0x02,0x00,0x00,0x6A,0x00 }); //Player 2 Top Right
		write(0x0063998E, { 0x68,0x1c,0x02,0x00,0x00,0x68,0x80,0x07,0x00,0x00,0x6A,0x00,0x68,0xc0,0x03,0x00,0x00 }); //Player 3 Bottom Left
		write(0x006399A9, { 0x68,0x38,0x04,0x00,0x00,0x68,0x80,0x07,0x00,0x00,0x68,0x1c,0x02,0x00,0x00,0x68,0xc0,0x03,0x00,0x00 }); //Player 4 Bottom Right
		write(0x00639F77, { 0x90,0xE9 }); //bypass crosshair

	}

	//Enable MultiSamping
	if (!config_disableFSAA)
	{
		set(0x00654C78 + 3, 0x08);
	}

	//fix some odd crashing bug
	write(0x00650210, { 0xC3 });

}
