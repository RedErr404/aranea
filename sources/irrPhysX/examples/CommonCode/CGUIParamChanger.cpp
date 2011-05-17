
#include "CGUIParamChanger.h"


//! constructor
CGUIParamChanger::CGUIParamChanger(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
: IGUIWindow(environment, parent, id, rectangle), Dragging(false)
{
	#ifdef _DEBUG
	setDebugName("CGUIParamChanger");
	#endif

	IGUISkin* skin = 0;
	if (environment)
		skin = environment->getSkin();

	IGUISpriteBank* sprites = 0;
	video::SColor color(255,255,255,255);
	
	s32 buttonw = 15;
	if (skin)
	{
		buttonw = skin->getSize(EGDS_WINDOW_BUTTON_WIDTH);
		sprites = skin->getSpriteBank();
		color = skin->getColor(EGDC_WINDOW_SYMBOL);
	}
	s32 posx = RelativeRect.getWidth() - buttonw - 4;

	CloseButton = Environment->addButton(core::rect<s32>(posx, 3, posx + buttonw, 3 + buttonw), this, -1, 
		L"", skin ? skin->getDefaultText(EGDT_WINDOW_CLOSE) : L"Close" );
	CloseButton->setSubElement(true);
	CloseButton->setTabStop(false);
	CloseButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	if (sprites)
	{
		CloseButton->setSpriteBank(sprites);
		CloseButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_WINDOW_CLOSE), color);
		CloseButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_WINDOW_CLOSE), color);
	}
	posx -= buttonw + 2;

	RestoreButton = Environment->addButton(core::rect<s32>(posx, 3, posx + buttonw, 3 + buttonw), this, -1, 
		L"", skin ? skin->getDefaultText(EGDT_WINDOW_RESTORE) : L"Restore" );
	RestoreButton->setVisible(false);
	RestoreButton->setSubElement(true);
	RestoreButton->setTabStop(false);
	RestoreButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	if (sprites)
	{
		RestoreButton->setSpriteBank(sprites);
		RestoreButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_WINDOW_RESTORE), color);
		RestoreButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_WINDOW_RESTORE), color);
	}
	posx -= buttonw + 2;

	MinButton = Environment->addButton(core::rect<s32>(posx, 3, posx + buttonw, 3 + buttonw), this, -1, 
		L"", skin ? skin->getDefaultText(EGDT_WINDOW_MINIMIZE) : L"Minimize" );
	MinButton->setVisible(false);
	MinButton->setSubElement(true);
	MinButton->setTabStop(false);
	MinButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	if (sprites)
	{
		MinButton->setSpriteBank(sprites);
		MinButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_WINDOW_MINIMIZE), color);
		MinButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_WINDOW_MINIMIZE), color);
	}

	MinButton->grab();
	RestoreButton->grab();
	CloseButton->grab();

	// this element is a tab group
	setTabGroup(true);
	setTabStop(true);
	setTabOrder(-1);
	
	NextParamPosY = 25;
	
	core::position2di bottomCentre(AbsoluteRect.getWidth()/2,AbsoluteRect.getHeight());
	//AbsoluteRect.addInternalPoint(core::position2d<s32>(bottomCentre.X + 25, bottomCentre.Y - 5));
	UpdateButton = Environment->addButton(core::rect<s32>(bottomCentre.X - 25,NextParamPosY, bottomCentre.X + 25, NextParamPosY+25), this, -1, L"Update");
	//updateAbsolutePosition();
	
}


//! destructor
CGUIParamChanger::~CGUIParamChanger()
{
	if (MinButton)
		MinButton->drop();

	if (RestoreButton)
		RestoreButton->drop();

	if (CloseButton)
		CloseButton->drop();
}


//! called if an event happened.
bool CGUIParamChanger::OnEvent(const SEvent& event)
{
	switch(event.EventType)
	{
	case EET_GUI_EVENT:
		if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUS_LOST)
		{
			Dragging = false;
		}
		else
		if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUSED)
		{
			if (event.GUIEvent.Caller == this && Parent)
			{
				Parent->bringToFront(this);
			}
		}
		else
		if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
		{
			if (event.GUIEvent.Caller == CloseButton)
			{
				if (Parent)
				{
					// send close event to parent
					SEvent e;
					e.EventType = EET_GUI_EVENT;
					e.GUIEvent.Caller = this;
					e.GUIEvent.Element = 0;
					e.GUIEvent.EventType = EGET_ELEMENT_CLOSED;

					// if the event was not absorbed
					if (!Parent->OnEvent(e))
					{
						remove();
					}
					return true;

				}
				else
				{
					remove();
					return true;
				}
			}
		}
		break;
	case EET_MOUSE_INPUT_EVENT:
		switch(event.MouseInput.Event)
		{
		case EMIE_LMOUSE_PRESSED_DOWN:
			DragStart.X = event.MouseInput.X;
			DragStart.Y = event.MouseInput.Y;
			Dragging = true;
			if (!Environment->hasFocus(this))
			{
				Environment->setFocus(this);
				if (Parent)
					Parent->bringToFront(this);
			}
			return true;
		case EMIE_LMOUSE_LEFT_UP:
			Dragging = false;
			return true;
		case EMIE_MOUSE_MOVED:
			if (Dragging)
			{
				// gui window should not be dragged outside its parent
				if (Parent)
					if (event.MouseInput.X < Parent->getAbsolutePosition().UpperLeftCorner.X +1 ||
						event.MouseInput.Y < Parent->getAbsolutePosition().UpperLeftCorner.Y +1 ||
						event.MouseInput.X > Parent->getAbsolutePosition().LowerRightCorner.X -1 ||
						event.MouseInput.Y > Parent->getAbsolutePosition().LowerRightCorner.Y -1)

						return true;
					

				move(core::position2d<s32>(event.MouseInput.X - DragStart.X, event.MouseInput.Y - DragStart.Y));
				DragStart.X = event.MouseInput.X;
				DragStart.Y = event.MouseInput.Y;
				return true;
			}
			break;
		default:
			break;
		}
	default:
		break;
	}

	return IGUIElement::OnEvent(event);
}


//! Updates the absolute position.
void CGUIParamChanger::updateAbsolutePosition()
{
	IGUIElement::updateAbsolutePosition();
}


//! draws the element and its children
void CGUIParamChanger::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();

	core::rect<s32> rect = AbsoluteRect;
	core::rect<s32> *cl = &AbsoluteClippingRect;

	// draw body fast
	rect = skin->draw3DWindowBackground(this, true, skin->getColor(EGDC_ACTIVE_BORDER),
		AbsoluteRect, &AbsoluteClippingRect);

	if (Text.size())
	{
		rect.UpperLeftCorner.X += skin->getSize(EGDS_TEXT_DISTANCE_X);
		rect.UpperLeftCorner.Y += skin->getSize(EGDS_TEXT_DISTANCE_Y);
		rect.LowerRightCorner.X -= skin->getSize(EGDS_WINDOW_BUTTON_WIDTH) + 5;

		IGUIFont* font = skin->getFont(EGDF_WINDOW);
		if (font)
			font->draw(Text.c_str(), rect, skin->getColor(EGDC_ACTIVE_CAPTION), false, true, cl);
	}

	IGUIElement::draw();
}


//! Returns pointer to the close button
IGUIButton* CGUIParamChanger::getCloseButton() const
{
	return CloseButton;
}


//! Returns pointer to the minimize button
IGUIButton* CGUIParamChanger::getMinimizeButton() const
{
	return MinButton;
}


//! Returns pointer to the maximize button
IGUIButton* CGUIParamChanger::getMaximizeButton() const
{
	return RestoreButton;
}

void CGUIParamChanger::addParam(wchar_t* name, f32 initialValue) {
     
     SParamInfo* paramInfo = new SParamInfo;
     Environment->addStaticText(name, core::rect<s32>(5,NextParamPosY,110,NextParamPosY+25), false, false, this);
     /*paramInfo->ScrollBar = Environment->addScrollBar(true, core::rect<s32>(55,NextParamPosY,RelativeRect.getWidth()-5,NextParamPosY+20), this);
     paramInfo->ScrollBar->setMax(maxValue);
     paramInfo->ScrollBar->setLargeStep(maxValue/10);
     paramInfo->ScrollBar->setSmallStep(1);*/
     core::stringw str(initialValue);
     paramInfo->EditBox = Environment->addEditBox(str.c_str(), core::rect<s32>(110,NextParamPosY,RelativeRect.getWidth()-5,NextParamPosY+20), true, this);
     paramInfo->Name = name;
     
     NextParamPosY += 25;
     
     UpdateButton->move(core::position2d<s32>(0,25));
     
     RelativeRect.addInternalPoint(core::position2d<s32>(RelativeRect.getWidth()+50,RelativeRect.getHeight()+50));
     updateAbsolutePosition();
     
     ParamInfos.push_back(paramInfo);     
     
}

bool CGUIParamChanger::getParam(wchar_t* name, f32& value) {
    
    for (u32 i = 0 ; i < ParamInfos.size() ; i++) 
        if (ParamInfos[i]->Name.equals_ignore_case(name)) {
            value = (f32)atof(core::stringc(ParamInfos[i]->EditBox->getText()).c_str());
            return true;                                  
        }
        
    return false;
     
}


