import { useState } from "react";
import { Joystick } from "react-joystick-component";
import { JoystickShape } from "react-joystick-component";
//import "./joystick.css"



// Function to convert joystick positions to wheel speeds
function joystickToWheelSpeeds(x, y) {
  var leftSpeed = 0.0;
  var rightSpeed = 0.0;
  // One joystick movement
  if (y === 0) {
    if (x >= 0) { // turn right on the spot
      leftSpeed = x * 15.0;
      rightSpeed = 0;
    } else { // turn left on the spot
      leftSpeed = 0;
      rightSpeed = -x * 15.0;
    }
  }
  else if (x === 0) {
    if (y >= 0) {
      leftSpeed = y * 15.0;
      rightSpeed = y * 15.0;
    } else { // backward binary
      leftSpeed = 0;
      rightSpeed = 1;
    }
  }
  // Two joystick movement
  else {
    var angle = (Math.atan2(y, x) * 180) / Math.PI; // Angle in degrees
    if (angle > -20 && angle <= 90) {
      leftSpeed = 15;
      angle = Math.max(angle, 0);
      rightSpeed = Math.floor((angle / 90) * 15); // 0 to 15
    } 
    
    else if ((angle > 90 && angle < 180) || (angle >= -180 && angle < -160)) {
      rightSpeed = 15;
      angle = Math.abs(angle) - 90;
      leftSpeed = 15 - Math.floor((angle / 90) * 15); // 15 to 0
    }   

    var ratioToMax = Math.max(Math.sqrt(x * x + y * y),1); //magnitude
    ratioToMax += 0.01; //prevents jittering at the edges of joystick
    if (angle > 0 && angle < 180) {
      rightSpeed /= ratioToMax;
      leftSpeed /= ratioToMax;
    }
  }
  return {
    leftWheelSpeed: leftSpeed,
    rightWheelSpeed: rightSpeed,
  };
}

export function Test() {
  const [joystickPos, setJoystickPos] = useState({ x: 0, y: 0 });
  const [binarySpeed, setBinarySpeed] = useState(119);
  const [leftWheelSpeed, setLeftWheelSpeed] = useState(0);
  const [rightWheelSpeed, setRightWheelSpeed] = useState(0);
  const [currTime, setCurrTime] = useState(Date.now());

  function setSpeed(leftWheelSpeed, rightWheelSpeed) {
    let combined8BitValue = (leftWheelSpeed << 4) | rightWheelSpeed;
    setLeftWheelSpeed((combined8BitValue >> 4) / 15.0);
    setRightWheelSpeed((combined8BitValue & 0b00001111) / 15.0);
    setBinarySpeed(combined8BitValue);
  }

  function callReq() {
    var request = new XMLHttpRequest();
    request.open('POST', 'http://192.168.114.249/data');
    request.send(JSON.stringify({ data: binarySpeed }))
  }

  function callStop() {
    var request = new XMLHttpRequest();
    request.open('POST', 'http://192.168.114.249/data');
    request.send(JSON.stringify({ data: 0 }))
  }

  const handleYMove = (stick) => {
    setJoystickPos({ x: joystickPos.x, y: stick.y });
    const { leftWheelSpeed, rightWheelSpeed } = joystickToWheelSpeeds(
      -joystickPos.y,
      joystickPos.x
    );
    setSpeed(leftWheelSpeed, rightWheelSpeed);
    if (Date.now() - currTime > 100) {
      setCurrTime(Date.now());
      callReq();
    }
  };

  const handleXMove = (stick) => {
    setJoystickPos({ x: stick.x, y: joystickPos.y });
    const { leftWheelSpeed, rightWheelSpeed } = joystickToWheelSpeeds(
      -joystickPos.y,
      joystickPos.x
    );
    setSpeed(leftWheelSpeed, rightWheelSpeed);
    if (Date.now() - currTime > 100) {
      setCurrTime(Date.now());
      callReq();
    }
  };

  const handleXStop = () => {
    setJoystickPos({x: 0  , y: joystickPos.y})
    const { leftWheelSpeed, rightWheelSpeed } = joystickToWheelSpeeds(
      -joystickPos.y,
      0
    );
    setSpeed(leftWheelSpeed, rightWheelSpeed);
    callStop();
  };

  const handleYStop = () => {
    setJoystickPos({x: joystickPos.x, y: 0})
    const { leftWheelSpeed, rightWheelSpeed } = joystickToWheelSpeeds(
      0,
      joystickPos.x
    );
    setSpeed(leftWheelSpeed, rightWheelSpeed);
    callStop();
  };

  return (
    <div id="elem-to-center">
      <Joystick controlPlaneShape={JoystickShape.AxisX} pos={{x: joystickPos.x, y:0}} move={handleXMove} stop={handleXStop} stickSize={100} size={300} />
      <Joystick controlPlaneShape={JoystickShape.AxisY} pos={{x:0, y:joystickPos.y}} move={handleYMove} stop={handleYStop} stickSize={100} size={300} />
      <p>Binary Speed: {binarySpeed}</p>
      <p>Left Speed in KL25: {leftWheelSpeed}</p>
      <p>Right Speed in KL25: {rightWheelSpeed}</p>
      <p>x: {joystickPos.x}</p>
      <p>y: {joystickPos.y}</p>
    </div>
  );
}