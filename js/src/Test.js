import { useState } from "react";
import { Joystick } from "react-joystick-component";
import "./joystick.css"

// Function to convert joystick positions to wheel speeds
function joystickToWheelSpeeds(x, y) {
  
  var leftWheelSpeed = y + x;
  var rightWheelSpeed = y - x;

  const angle = Math.atan2(y, x) / Math.PI * 180;
  console.log(angle)
  if (angle < 0) {
    if (angle <= -30 && angle >= -150) {
      leftWheelSpeed = rightWheelSpeed = y;
    } else {
      leftWheelSpeed = x;
      rightWheelSpeed = -x;
    }
  }
  
  // Avoid division by zero in normalization
  // const angle = Math.atan2(y, x) % (Math.PI / 2);
  // var maxSpeed = Math.sqrt(2) * Math.sin(angle + Math.PI / 4);
  const maxSpeed = Math.max(Math.abs(leftWheelSpeed), Math.abs(rightWheelSpeed), 1);
  if (maxSpeed !== 0) {
    leftWheelSpeed /= maxSpeed;
    rightWheelSpeed /= maxSpeed;
  }

  return {
    leftWheelSpeed: leftWheelSpeed,
    rightWheelSpeed: rightWheelSpeed,
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
    request.send(JSON.stringify({data : binarySpeed}))
  }

  function callStop() {
    var request = new XMLHttpRequest();
    request.open('POST', 'http://192.168.114.249/data');
    request.send(JSON.stringify({data : 119}))
  }

  const handleMove = (stick) => {
    setJoystickPos({ x: stick.x, y: stick.y });

    const { leftWheelSpeed, rightWheelSpeed } = joystickToWheelSpeeds(
      stick.x,
      stick.y
    );
    
    // Convert wheel speeds to 4-bit values
    const left4Bit = Math.round((leftWheelSpeed + 1) * 7.5);
    const right4Bit = Math.round((rightWheelSpeed + 1) * 7.5);
    const combined8BitValue = (left4Bit << 4) | right4Bit;
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
    setBinarySpeed(119);
    setLeftWheelSpeed(0);
    setRightWheelSpeed(0);
    callStop();
  };

  return (
    <div id="elem-to-center">
      <Joystick pos={joystickPos} move={handleMove} stop={handleStop} stickSize={100} size={300}/>
      <p>Binary Speed: {binarySpeed}</p>
      <p>Left Speed in KL25: {leftWheelSpeed}</p>
      <p>Right Speed in KL25: {rightWheelSpeed}</p>
    </div>
  );
}
