// import logo from './logo.svg';
import React, { Component } from "react";
import { Routes, Route, Link } from "react-router-dom";
import "bootstrap/dist/css/bootstrap.min.css";
import './App.css';

import Add from "./components/Add";
import Search from "./components/Search";
import List from "./components/List";

function App() {
  return (
    <div>
        <nav className="navbar navbar-expand navbar-dark bg-dark">
          <Link to={"/tutorials"} className="navbar-brand m-1">
            Home
          </Link>
          <div className="navbar-nav mr-auto">
            <li className="nav-item">
              <Link to={"/tutorials"} className="nav-link">
                Lists
              </Link>
            </li>
            <li className="nav-item">
              <Link to={"/add"} className="nav-link">
                Add
              </Link>
            </li>
          </div>
        </nav>

        <div className="container mt-2">
          <Routes>
            <Route path="/" element={<List/>} />
            <Route path="/tutorials" element={<List/>} />
            <Route path="/add" element={<Add/>} />
            <Route path="/tutorials/:id" element={<Search/>} />
          </Routes>
        </div>
      </div>
  );
}

export default App;
