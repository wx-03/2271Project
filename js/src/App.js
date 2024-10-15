import React from 'react';
import { Routes, Route, BrowserRouter } from "react-router-dom";
import Data from "./data";
import { Test } from './Test';

function App() {
  return (
    <>
      <BrowserRouter>
        <Routes>
          <Route path="/" element={<Test />} />
          <Route path="/data" element={<Test />} />
        </Routes>
      </BrowserRouter>
    </>
  )
}

export default App;
