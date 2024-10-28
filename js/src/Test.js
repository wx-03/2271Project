import { useState } from "react";
import { Joystick } from "react-joystick-component";
import { JoystickShape } from "react-joystick-component";
//import "./joystick.css"

// Function to convert joystick positions to wheel speeds
function joystickToWheelSpeeds(x, y) {
  // Angle in degrees
  var angle = (Math.atan2(y, x) * 180) / Math.PI;
  // var maxSpeed = Math.sqrt(2) * Math.sin(angle + Math.PI / 4);
  var leftSpeed = 0.0;
  var rightSpeed = 0.0;
  if (angle > 0 && angle <= 90) {
    leftSpeed = 15
    rightSpeed = Math.floor((angle / 90) * 8) + 7;
  } else if (angle > 90 && angle < 180) {
    rightSpeed = 15;
    angle -= 90;
    leftSpeed = 8 - Math.floor((angle / 90) * 8) + 7;
    if (leftSpeed === 0 && rightSpeed === 1) {
      //forgo this small movement for move backwards
      rightSpeed = 0;
    }
  } else if (angle <= 0 & angle > -20) { //extra for turn on spot
    leftSpeed = 15;
    rightSpeed = 0;
  } else if (angle >= -180 && angle < -160) { //extra for turn on spot
    leftSpeed = 0;
    rightSpeed = 15;
  } else {
    //backward binary
    leftSpeed = 0;
    rightSpeed = 1;
  }

  //00000000 will be for stop

  var ratioToMax = Math.sqrt(x * x + y * y); //magnitude
  ratioToMax += 0.01; //prevents jittering at the edges of joystick
  if (angle > 0 && angle < 180) {
    rightSpeed *= ratioToMax;
    leftSpeed *= ratioToMax;
  }

  if (rightSpeed === 1 && leftSpeed === 0) {
    rightSpeed = 2;
  }

  if (angle < -20 && angle > -160) {
    leftSpeed = 0;
    rightSpeed = 1;
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

    const combined8BitValue = (leftWheelSpeed << 4) | rightWheelSpeed;
    setLeftWheelSpeed((combined8BitValue >> 4) / 15.0);
    //setLeftWheelSpeed(leftWheelSpeed);
    setRightWheelSpeed((combined8BitValue & 0b00001111) / 15.0);
    //setRightWheelSpeed(rightWheelSpeed);
    setBinarySpeed(combined8BitValue);
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

    const combined8BitValue = (leftWheelSpeed << 4) | rightWheelSpeed;
    setLeftWheelSpeed((combined8BitValue >> 4) / 15.0);
    //setLeftWheelSpeed(leftWheelSpeed);
    setRightWheelSpeed((combined8BitValue & 0b00001111) / 15.0);
    //setRightWheelSpeed(rightWheelSpeed);
    setBinarySpeed(combined8BitValue);
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
    const combined8BitValue = (leftWheelSpeed << 4) | rightWheelSpeed;
    setLeftWheelSpeed((combined8BitValue >> 4) / 15.0);
    setRightWheelSpeed((combined8BitValue & 0b00001111) / 15.0);
    setBinarySpeed(combined8BitValue);
    callStop();
  };

  const handleYStop = () => {
    setJoystickPos({x: joystickPos.x, y: 0})
    const { leftWheelSpeed, rightWheelSpeed } = joystickToWheelSpeeds(
      0,
      joystickPos.x
    );
    const combined8BitValue = (leftWheelSpeed << 4) | rightWheelSpeed;
    setLeftWheelSpeed((combined8BitValue >> 4) / 15.0);
    setRightWheelSpeed((combined8BitValue & 0b00001111) / 15.0);
    setBinarySpeed(combined8BitValue);
    callStop();
  };

  return (
    <div id="elem-to-center">
      <Joystick controlPlaneShape={JoystickShape.AxisX} pos={{x: joystickPos.x, y:0}} move={handleXMove}  stickSize={100} size={300} />
      <Joystick controlPlaneShape={JoystickShape.AxisY} pos={{x:0, y:joystickPos.y}} move={handleYMove} stickSize={100} size={300} />
      <p>Binary Speed: {binarySpeed}</p>
      <p>Left Speed in KL25: {leftWheelSpeed}</p>
      <p>Right Speed in KL25: {rightWheelSpeed}</p>
    </div>
  );
}