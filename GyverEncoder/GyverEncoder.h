#pragma once
#include <Arduino.h>

/*	
	GyverEncoder - библиотека для отработки энкодера. Возможности:	
	- Отработка поворота энкодера
	- Отработка "нажатого поворота"	
	- Отработка "быстрого поворота"
	- Несколько алгоритмов опроса энкодера
	- Выбор подтяжки подключения энкодера
	- Работа с двумя типами экнодеров
	- Работа с внешним энкодером (через расширитель пинов и т.п.)
	- Отработка нажатия/удержания кнопки с антидребезгом	
	
	Документацию читай здесь: https://alexgyver.ru/encoder/
	Для максимально быстрого опроса энкодера рекомендуется использовать ядро GyverCore https://alexgyver.ru/gyvercore/
	
	Версии:
	- 3.6 от 16.09.2019	- Возвращены дефайны настроек
	- 4.0 от 13.11.2019
		- Оптимизирован код
		- Исправлены баги
		- Добавлены другие алгоритмы опроса
		- Добавлена возможность полностью убрать кнопку (экономия памяти)
		- Добавлена возможность подключения внешнего энкодера
		- Добавлена настройка подтяжки пинов
*/

// =========== НАСТРОЙКИ ===========
// закомментируй строку, чтобы полностью убрать отработку кнопки из кода
#define ENC_WITH_BUTTON

// тип подключения энкодера по умолчанию (INPUT или INPUT_PULLUP)
#define DEFAULT_PULL INPUT

// тип подключения кнопки энкодера по умолчанию (INPUT или INPUT_PULLUP)
#define DEFAULT_BTN_PULL INPUT_PULLUP

// алгоритмы опроса энкодера (раскомментировать нужный)
//#define FAST_ALGORITHM		// быстрый, не справляется с люфтами
//#define BINARY_ALGORITHM		// медленнее, лучше справляется с люфтами
#define PRECISE_ALGORITHM		// медленнее, но работает даже с убитым энкодером (по мотивам https://github.com/mathertel/RotaryEncoder)

// настройка антидребезга энкодера, кнопки и таймаута удержания
#define ENC_DEBOUNCE_TURN 0
#define ENC_DEBOUNCE_BUTTON 80
#define ENC_HOLD_TIMEOUT 700

#pragma pack(push,1)
typedef struct
{	
	bool hold_flag: 1;
	bool butt_flag: 1;
	bool turn_flag: 1;
	bool isTurn_f: 1;
	bool isPress_f: 1;
	bool isRelease_f: 1;
	bool isHolded_f: 1;
	bool isFastR_f: 1;
	bool isFastL_f: 1;
	bool enc_tick_mode: 1;
	bool enc_type: 1;
	bool use_button : 1;
	bool extCLK : 1;
	bool extDT : 1;
	bool extSW : 1;
} GyverEncoderFlags;
#pragma pack(pop)

#define ENC_NO_BUTTON -1	// константа для работы без пина
#define TYPE1 0		// полушаговый энкодер
#define TYPE2 1		// полношаговый
#define NORM 0		// направление вращения обычное
#define REVERSE 1	// обратное
#define MANUAL 0	// нужно вызывать функцию tick() вручную
#define AUTO 1		// tick() входит во все остальные функции и опрашивается сама!
#define HIGH_PULL 0
#define LOW_PULL 1

// Варианты инициализации:
// Encoder enc;									// не привязан к пину
// Encoder enc(пин CLK, пин DT);				// энкодер без кнопки (ускоренный опрос)
// Encoder enc(пин CLK, пин DT, пин SW);		// энкодер с кнопкой
// Encoder enc(пин CLK, пин DT, пин SW, тип);	// энкодер с кнопкой и указанием типа
// Encoder enc(пин CLK, пин DT, ENC_NO_BUTTON, тип);	// энкодер без кнопкой и с указанием типа

class Encoder {
public:
	Encoder();								// для непривязанного к пинам энкодера
	Encoder(uint8_t clk, uint8_t dt, int8_t sw = -1, bool type = false); // CLK, DT, SW, тип (TYPE1 / TYPE2) TYPE1 одношаговый, TYPE2 двухшаговый. Если ваш энкодер работает странно, смените тип
	
	void tick();							// опрос энкодера, нужно вызывать постоянно или в прерывании
	void tick(bool clk, bool dt, bool sw = 0);	// опрос "внешнего" энкодера
	void setType(bool type);				// TYPE1 / TYPE2 - тип энкодера TYPE1 одношаговый, TYPE2 двухшаговый. Если ваш энкодер работает странно, смените тип
	void setPinMode(bool mode);				// тип подключения энкодера, подтяжка HIGH_PULL (внутренняя) или LOW_PULL (внешняя на GND)
	void setTickMode(bool tickMode); 		// MANUAL / AUTO - ручной или автоматический опрос энкодера функцией tick(). (по умолчанию ручной)
	void setDirection(bool direction);		// NORM / REVERSE - направление вращения энкодера
	void setFastTimeout(uint16_t timeout);	// установка таймаута быстрого поворота
	
	boolean isTurn();						// возвращает true при любом повороте, сама сбрасывается в false
	boolean isRight();						// возвращает true при повороте направо, сама сбрасывается в false
	boolean isLeft();						// возвращает true при повороте налево, сама сбрасывается в false
	boolean isRightH();						// возвращает true при удержании кнопки и повороте направо, сама сбрасывается в false
	boolean isLeftH();						// возвращает true при удержании кнопки и повороте налево, сама сбрасывается в false
	boolean isFastR();						// возвращает true при быстром повороте
	boolean isFastL();						// возвращает true при быстром повороте
	
	boolean isPress();						// возвращает true при нажатии кнопки, сама сбрасывается в false
	boolean isRelease();					// возвращает true при отпускании кнопки, сама сбрасывается в false
	boolean isClick();						// возвращает true при нажатии и отпускании кнопки, сама сбрасывается в false
	boolean isHolded();						// возвращает true при удержании кнопки, сама сбрасывается в false
	boolean isHold();						// возвращает true при удержании кнопки, НЕ СБРАСЫВАЕТСЯ
	
private:
	GyverEncoderFlags flags;
	uint8_t _fast_timeout = 50;				// таймаут быстрого поворота
	uint8_t prevState = 0;
	uint8_t encState = 0;	// 0 не крутился, 1 лево, 2 право, 3 лево нажат, 4 право нажат
	uint32_t debounce_timer = 0, fast_timer = 0;
	uint8_t _CLK = 0, _DT = 0, _SW = 0;
	bool turnFlag = false, extTick = false, SW_state = false;
};