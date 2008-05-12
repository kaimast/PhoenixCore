/*

Copyright (c) 2007, Jonathan Wayne Parrott.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include "PhSpriteAnimation.h"

using namespace phoenix;

PhSpriteAnimation::PhSpriteAnimation(PhSceneManager* s)
	: PhSceneNode(0.0f), texture(NULL), framesize(0,0), frame(0), framerate(0), rot(0.0f), mMin(0), mMax(0), flip(false),
	color(255,255,255), pos(0,0), smgr(s), enabled(true)
{
    smgr->addNode(this);
}

PhSpriteAnimation::PhSpriteAnimation(PhSceneManager* s, PhTexture* t, PhVector2d a, PhVector2d p)
	: PhSceneNode(0.0f), texture(t), framesize(a), frame(0), framerate(1.0f), rot(0.0f), mMin(0), mMax(0), flip(false),
	color(255,255,255), pos(p), smgr(s), enabled(true)
{
    smgr->addNode(this);
}

PhSpriteAnimation::~PhSpriteAnimation()
{
    smgr->removeNode(this);
}

void PhSpriteAnimation::setTexture(PhTexture* t)
{
    texture = t;
}

PhTexture* PhSpriteAnimation::getTexture()
{
    return texture;
}

void PhSpriteAnimation::setAnimation(int b, int e)
{
    mMin = b;
    mMax = e;
    if (frame > mMax)
    {
        frame = (float)mMin;
    }
    if (frame < mMin)
    {
        frame = (float)mMax;
    }
}

void PhSpriteAnimation::setFrame(int a)
{
    frame = float(a);
}

int PhSpriteAnimation::getFrame()
{
    return int(floor(frame));
}

void PhSpriteAnimation::setAnimationSpeed(float a)
{
    framerate = a;
}

float PhSpriteAnimation::getAnimationSpeed()
{
    return framerate;
}

void PhSpriteAnimation::setFrameSize(PhVector2d s)
{
    framesize = s;
}

PhVector2d PhSpriteAnimation::getFrameSize()
{
    return framesize;
}

//! Set position.
/*!
    \param s The new position.
    \sa getPosition()
*/
void PhSpriteAnimation::setPosition(PhVector2d s)
{
    pos = s;
}

//! Get position.
/*!
    \return The current position.
    \sa setPosition()
*/
PhVector2d PhSpriteAnimation::getPosition()
{
    return pos;
}

//! Get rotation.
/*!
    \return The current rotation (in degrees).
*/
float PhSpriteAnimation::getRotation()
{
    return rot;
}

//! Set rotation.
/*!
    \param r New rotation (in degrees).
*/
void PhSpriteAnimation::setRotation(float r)
{
    rot = r;
}

//! Get flip.
/*!
    \return If the sprite is being flipped.
*/
bool PhSpriteAnimation::getFlip()
{
    return flip;
}

//! Set flip.
/*!
    \param f Pass true to flip the sprite.
*/
void PhSpriteAnimation::setFlip(bool f)
{
    flip = f;
}

//! Get color.
/*!
    \return The current blending color.
*/
PhColor PhSpriteAnimation::getColor()
{
    return color;
}

//! Set color.
/*!
    \param c The new blending color.
*/
void PhSpriteAnimation::setColor(PhColor c)
{
    color = c;
}

//! Enable
/*!
    PhSpriteAnimation::Enables this sprite.
*/
void PhSpriteAnimation::enable() {
    enabled = true;
}

//! Disable
/*!
    PhSpriteAnimation::Disables this sprite.
*/
void PhSpriteAnimation::disable() {
    enabled = false;
}

void PhSpriteAnimation::onPreRender()
{
    if(enabled) {
        smgr->registerForRendering(this);
    }
}

void PhSpriteAnimation::onRender()
{

    float x,y;
    x =  floor(frame) * framesize.getX();
    y = (floor(frame/floor((texture->getWidth()/framesize.getX())))) * framesize.getY();

    smgr->getRenderSystem()->drawTexturePart( texture, pos, PhRect( x, y, framesize.getX(), framesize.getY() ), depth, rot, 1.0f, color, flip );

}

void PhSpriteAnimation::onPostRender()
{

    //advance the animation
    frame += framerate;
    if (frame > mMax)
    {
        frame = (float)mMin;
    }
    if (frame < mMin)
    {
        frame = (float)mMax;
    }

}
