import { Typography, Button } from '@mui/material';
import React, { useEffect, useState } from 'react';
import CountdownTimer from './CountdownTimer';
import ColorBox from './ColorBox';
import app from '../utils/firebase';
import { getDatabase, ref, onValue } from 'firebase/database';
import { Link } from 'react-router-dom';

function genColor() {
	const r = Math.floor(Math.random() * 256);
	const g = Math.floor(Math.random() * 256);
	const b = Math.floor(Math.random() * 256);

	return { r, g, b };
}

function checkColor(color1, color2) {
	const { r: r1, g: g1, b: b1 } = color1;
	const { r: r2, g: g2, b: b2 } = color2;
	const diffR = Math.abs(r1 - r2);
	const diffG = Math.abs(g1 - g2);
	const diffB = Math.abs(b1 - b2);
	return diffR < 25 && diffG < 25 && diffB < 25;
}

export default function Game() {
	const [isCorrect, setIsCorrect] = useState(false);
	const [colorFromUser, setColorFromUser] = useState({
		r: 0,
		g: 0,
		b: 0,
	});
	const [gameColor, setGameColor] = useState({ r: 255, g: 255, b: 255 });
	const [timestamp, setTimestamp] = useState(0);
	const [isOver, setIsOver] = useState(false);

	function handleCorrectAnswer() {
		setIsCorrect(false);
		console.log('correct');
		const newColor = genColor();
		setGameColor((prevState) => {
			return { ...prevState, ...newColor };
		});
	}

	function update() {
		var userId = 'XJI27hbv5eVmvEXTaCJTQnhZ33C2';
		const dbRef = ref(getDatabase(app));

		onValue(dbRef, function(snapshot) {
			const data = snapshot.val().UsersData[userId].readings;

			const dataColor = data.data.split(' ');
			// console.log(dataColor);
			if (timestamp !== data.timestamp) {
				setColorFromUser((prevState) => ({
					...prevState,
					r: Number(dataColor[0]),
					g: Number(dataColor[1]),
					b: Number(dataColor[2]),
				}));
				setTimestamp(data.timestamp);
			}
		});
	}

	useEffect(() => {
		handleCorrectAnswer();
	}, []);

	useEffect(() => {
		if (checkColor(colorFromUser, gameColor)) {
			setIsCorrect(true);
		}
		update();
		// handleCorrectAnswer();
		console.log('---------------');
		console.log('gamecolor : ', gameColor);
		console.log('color from user: ', colorFromUser);
		console.log(timestamp);
		console.log('---------------');
	}, [timestamp]);

	// useEffect(() => {
	// 	console.log(gameColor);
	// 	console.log(colorFromUser);
	// 	console.log(timestamp);
	// }, [timestamp]);

	return (
		<div className="main-container">
			<div className="main">
				<Typography
					variant="h1"
					sx={{
						color: 'white',
						fontSize: '20px',
						padding: '2rem',
					}}
				>
					FIND AN ITEM THAT IS THE SAME COLOR AS THE COLOR IN THE BOX.
				</Typography>
				<div className="current-color">
					<ColorBox
						color={`rgb(${gameColor.r}, ${gameColor.g}, ${gameColor.b})`}
					/>
					{!isOver ? (
						<CountdownTimer
							isCorrect={isCorrect}
							handleCorrectAnswer={handleCorrectAnswer}
							setIsCorrect={setIsCorrect}
							setIsOver={setIsOver}
						/>
					) : (
						<div className="gameover">
							<Typography
								className="gameovertext"
								sx={{ fontSize: '22px' }}
							>
								Game Over
							</Typography>
							<div className="button-end">
								<Button
									variant="outlined"
									sx={{ mt: 2, mr: 1 }}
									onClick={() => {
										setIsOver(false);
										const newColor = genColor();
										setGameColor((prevState) => {
											return {
												...prevState,
												...newColor,
											};
										});
									}}
								>
									Restart
								</Button>
								<Button
									component={Link}
									to="/"
									variant="outlined"
									sx={{ mt: 2 }}
								>
									Main Menu
								</Button>
							</div>
						</div>
					)}
				</div>

				<Typography
					sx={{
						color: 'white',
						fontSize: '17px',
						padding: '1rem',
					}}
				>
					FOUNT IT?
				</Typography>
				<Typography sx={{ color: 'white', fontSize: '17px' }}>
					• CLAP 3 TIMES
				</Typography>
				<Typography sx={{ color: 'white', fontSize: '17px' }}>
					• PLACE THE ITEM IN FRONT OF THE SENSOR
				</Typography>
			</div>
		</div>
	);
}
