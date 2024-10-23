import { useState } from "react";
import { Joystick } from "react-joystick-component";
//import "./joystick.css"

// Function to convert joystick positions to wheel speeds
function joystickToWheelSpeeds(x, y) {
  // Angle in degrees
  var angle = (Math.atan2(y, x) * 180) / Math.PI;
  var maxSpeed = Math.sqrt(2) * Math.sin(angle + Math.PI / 4);

  var leftSpeed = 0;
  var rightSpeed = 0;
  if (angle > 0 && angle <= 90) {
    leftSpeed = 15
    rightSpeed = Math.floor((angle / 90) * 8) + 7;
  } else if (angle > 90 && angle < 180) {
    rightSpeed = 15;
    angle -= 90;
    leftSpeed = 8 - Math.floor((angle / 90) * 8) + 7;
    if (leftSpeed == 0 && rightSpeed == 1) {
      //forgo this small movement for move backwards
      rightSpeed = 0;
    }
  } else if (angle < 0 & angle > -20) { //extra for turn on spot
    leftSpeed = 15;
    rightSpeed = 0;
  } else if (angle > -180 && angle < -160) { //extra for turn on spot
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

  if (rightSpeed == 1 && leftSpeed == 0) {
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

  const handleMove = (stick) => {
    setJoystickPos({ x: stick.x, y: stick.y });

    const { leftWheelSpeed, rightWheelSpeed } = joystickToWheelSpeeds(
      stick.x,
      stick.y
    );

    // Convert wheel speeds to 4-bit values
    //const left4Bit = Math.round((leftWheelSpeed + 1) * 7.5);
    //const right4Bit = Math.round((rightWheelSpeed + 1) * 7.5);
    const combined8BitValue = (leftWheelSpeed << 4) | rightWheelSpeed;
    setLeftWheelSpeed(((combined8BitValue >> 4) - 7) / 8.0);
    setRightWheelSpeed(((combined8BitValue & 0b00001111) - 7) / 8.0);

    //const binaryValue = combined8BitValue.toString(2).padStart(8, "0");
    setBinarySpeed(combined8BitValue);
    if (Date.now() - currTime > 100) {
      setCurrTime(Date.now());
      callReq();
    }
  };

  const handleStop = () => {
    setJoystickPos({ x: 0, y: 0 });
    setBinarySpeed(0);
    setLeftWheelSpeed(0);
    setRightWheelSpeed(0);
    callStop();
  };

  return (
    <div id="elem-to-center">
      <Joystick pos={joystickPos} move={handleMove} stop={handleStop} stickSize={100} size={300} />
      <p>Binary Speed: {binarySpeed}</p>
      <p>Left Speed in KL25: {leftWheelSpeed}</p>
      <p>Right Speed in KL25: {rightWheelSpeed}</p>
    </div>
  );
}