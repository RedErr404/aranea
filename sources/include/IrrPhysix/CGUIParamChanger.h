
#ifndef INC_CGUIPARAMCHANGER_H
#define INC_CGUIPARAMCHANGER_H

#include <irrlicht.h>

using namespace irr;
using namespace gui;


class CGUIParamChanger : public IGUIWindow
	{
	public:

		//! constructor
		CGUIParamChanger(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);

		//! destructor
		virtual ~CGUIParamChanger();

		//! called if an event happened.
		virtual bool OnEvent(const SEvent& event);

		//! update absolute position
		virtual void updateAbsolutePosition();

		//! draws the element and its children
		virtual void draw();

		//! Returns pointer to the close button
		virtual IGUIButton* getCloseButton() const;

		//! Returns pointer to the minimize button
		virtual IGUIButton* getMinimizeButton() const;

		//! Returns pointer to the maximize button
		virtual IGUIButton* getMaximizeButton() const;
		
		IGUIButton* getUpdateButton() const { return UpdateButton; }
		
		void addParam(wchar_t* name, f32 initialValue);
		bool getParam(wchar_t* name, f32& value);

	private:
        struct SParamInfo {
            SParamInfo() {
                Name = "";
                //ScrollBar = 0;
                EditBox = 0;
            }
            core::stringw Name;
            //IGUIScrollBar* ScrollBar;     
            IGUIEditBox* EditBox;
        };

		core::position2d<s32> DragStart;
		bool Dragging;

		IGUIButton* CloseButton;
		IGUIButton* MinButton;
		IGUIButton* RestoreButton;
		
		core::array<SParamInfo*> ParamInfos;
		s32 NextParamPosY;
		IGUIButton* UpdateButton;
	};

#endif // INC_CGUIPARAMCHANGER_H


