#include <Esplora.h>
#include <TFT.h>
#include <SPI.h>

int screenX = EsploraTFT.width();
int screenY = EsploraTFT.height();
int ballX = screenX / 2;
int ballY = screenY / 2;
int ballLastX = ballX;
int ballLastY = ballY;
int ballXDir = 1;
int ballYDir = -2;
int ballRadius = 5;
int ballYmax = 8;
unsigned long time;
unsigned long waitUntil;
int ballDelay = 30;
int paddleX = screenX / 2;
int paddleY = screenY - 5;
int paddleLastX = ballX;
int paddleW = 20;
const int bricksWide = 14;
const int bricksTall = 10;
const int brickW = 10;
const int brickH = 5;
int totalBricks = 0;
int bricksHit = 0;
boolean brick[bricksWide][bricksTall];
boolean sound = HIGH;

void setup(){
  // initialize the display
  EsploraTFT.begin();
  // set the background the black
  EsploraTFT.background(0,0,0);
  newScreen();
}
void loop(){
  paddle();
  checkSound();
  time=millis();
  if (time>waitUntil){
    waitUntil=time+ballDelay;
    moveBall();
  }
}

void paddle(){
  paddleX=map(Esplora.readSlider(),0,1023,screenX,0)-paddleW/2;
  if (paddleX<1){  //dont let the paddle go to far left
    paddleX=1;
  }
  if (paddleX>screenX-paddleW){  //dont let the paddle go to far right
    paddleX=screenX-paddleW;
  }
  if (paddleX != paddleLastX) {
    EsploraTFT.fill(0,0,0);        //erase the old paddle
    EsploraTFT.rect(paddleLastX,paddleY,paddleW,4);
    EsploraTFT.fill(255,255,255);  //draw the new paddle
    EsploraTFT.rect(paddleX,paddleY,paddleW,4);
    paddleLastX=paddleX;
  }
}

void checkSound(void){
  if (Esplora.readButton(SWITCH_LEFT) == LOW){
    sound = !sound;
    delay(250);
  }
}

void moveBall(void){
  //check if the ball hits the side walls
  if (ballX<ballXDir*-1 | ballX>screenX-ballXDir-ballRadius){
    ballXDir = -ballXDir;
    if (sound == HIGH){
      Esplora.tone(230,10);
    }
  }
  //check if the ball hits the top of the screen
  if (ballY<ballYDir*-1){
    ballYDir = -ballYDir;
    if (sound == HIGH){
      Esplora.tone(530,10);
    }
  }
  //check if ball hits bottom of bricks
  for (int a=0; a<bricksWide; a++){
    for (int b=0; b<bricksTall; b++){
      if (brick[a][b]==HIGH){
        //if (ballX>a*20+10-ballRadius & ballX<a*20+29+ballRadius & ballY>b*10+20-ballRadius & ballY<b*10+29+ballRadius+ballRadius){
        if (ballX>a*brickW+10-ballRadius & ballX<a*brickW+brickW+10 & ballY>b*brickH+10-ballRadius & ballY<b*brickH+brickH+10){
          //Tft.fillRectangle(a*20+10,b*10+20,19,9,0);
          EsploraTFT.fill(0,0,0);
          EsploraTFT.rect(a*brickW+10,b*brickH+10,brickW,brickH);
          brick[a][b]=LOW;
          ballYDir = -ballYDir;
          bricksHit=bricksHit+1;
          if (sound == HIGH){
            Esplora.tone(330,10);
          }
        }
      }
    }
  }
  //check if the ball hits the paddle
  if (ballX>paddleX-ballXDir-ballRadius & ballX<paddleX+paddleW+ballXDir*-1 & ballY>paddleY-ballYDir-ballRadius & ballY<paddleY){
    ballXDir = ballXDir-(((paddleX+paddleW/2-ballRadius/2)-ballX)*.3);
    if (ballXDir<-ballYmax){
      ballXDir=-ballYmax;
    }
    else if (ballXDir>ballYmax){
      ballXDir=ballYmax;
    }
    ballYDir = -ballYDir;
    if (sound == HIGH){
    Esplora.tone(730,10);
    }
  }
  //check if the ball went past the paddle
  if (ballY>paddleY+10){
    if (sound == HIGH){
    Esplora.tone(130,1000);
    }
    for (int i=0; i<200; i++){
      EsploraTFT.stroke(0,255,0);
      EsploraTFT.text("OOPS",30,100);
      EsploraTFT.stroke(255,0,0);
      EsploraTFT.text("OOPS",30,100);
    }
    EsploraTFT.stroke(0,0,0);
    delay(2000);
    newScreen();
    ballY=screenY / 2;
    bricksHit = 0;
  }
  //check if there are any more bricks
  if (bricksHit==totalBricks){
    EsploraTFT.fill(0,0,0);
    EsploraTFT.rect(ballX,ballY,ballRadius,ballRadius);
    if (sound == HIGH){
    Esplora.tone(400,250);
    delay(250);
    Esplora.tone(415,250);
    delay(250);
    Esplora.tone(430,500);
    }
    for (int i=0; i<500; i++){
      EsploraTFT.stroke(0,255,0);
      EsploraTFT.text("YOU WIN",30,100);
      EsploraTFT.stroke(0,0,255);
      EsploraTFT.text("YOU WIN",30,100);
    }
    EsploraTFT.stroke(0,0,0);
    delay(2000);
    newScreen();
    ballY = screenY / 2;
    ballX = screenX / 2;
    bricksHit = 0;
  }
  //calculate the new position for the ball
  ballX=ballX+ballXDir;  //move the ball x
  ballY=ballY+ballYDir;  //move the ball y
  //erase the old ball
  EsploraTFT.fill(0,0,0);
  EsploraTFT.rect(ballLastX,ballLastY,ballRadius,ballRadius);
  // draw the new ball
  EsploraTFT.fill(255,255,255);
  EsploraTFT.rect(ballX, ballY,ballRadius,ballRadius);
  //update the last ball position to the new ball position
  ballLastX = ballX;
  ballLastY = ballY;
}

void newScreen(void){
  EsploraTFT.background(0,0,0);
  paddleLastX = ballX;
  paddle();
  blocks();
}

void blocks(void){
  totalBricks = 0;
  for (int a=0; a<bricksWide; a++){
    for (int b=0; b<bricksTall; b++){
      brick[a][b]=HIGH;
    }
  }
  for (int a=0; a<bricksWide; a++){
    for (int b=0; b<bricksTall; b++){
      int c = map(b,0,bricksWide,0,255);
      if (brick[a][b] == HIGH) {
        totalBricks += 1;
        EsploraTFT.fill(c,255-c/2,255);
        EsploraTFT.rect(a*brickW+10,b*brickH+10,brickW,brickH);
      }
    }
  }
}











