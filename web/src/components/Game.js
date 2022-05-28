import { Typography, Button, Container } from '@mui/material';
import React, { useEffect, useState } from 'react';
import CountdownTimer from './CountdownTimer';
import ColorBox from './ColorBox';
import app from '../utils/firebase';
import { getDatabase, ref, onValue } from 'firebase/database';
import { Link } from 'react-router-dom';

const RGBThreshold = 7000;
const HueThreshold = 30;
const SatThreshold = 70;
const Ligthreshold = 40;

function genColor() {
	const r = Math.floor(Math.random() * 256);
	const g = Math.floor(Math.random() * 256);
	const b = Math.floor(Math.random() * 256);

	return { r, g, b };
}

function RGBToHSL(r, g, b) {
	r /= 255;
	g /= 255;
	b /= 255;
	const l = Math.max(r, g, b);
	const s = l - Math.min(r, g, b);
	const h = s
		? l === r
			? (g - b) / s
			: l === g
			? 2 + (b - r) / s
			: 4 + (r - g) / s
		: 0;
	return [
		60 * h < 0 ? 60 * h + 360 : 60 * h,
		100 * (s ? (l <= 0.5 ? s / (2 * l - s) : s / (2 - (2 * l - s))) : 0),
		(100 * (2 * l - s)) / 2,
	];
}

function checkRGB(r1, g1, b1, r2, g2, b2) {
	const diffR = Math.abs(r1 - r2);
	const diffG = Math.abs(g1 - g2);
	const diffB = Math.abs(b1 - b2);
	return diffR * diffR + diffG * diffG + diffB * diffB <= RGBThreshold;
}

function getHueDistance(h1, h2) {
	let diffH = Math.abs(h1 - h2);
	return diffH > 180 ? 360 - diffH : diffH;
}

function checkHSL(r1, g1, b1, r2, g2, b2) {
	const [h1, s1, l1] = RGBToHSL(r1, g1, b1);
	const [h2, s2, l2] = RGBToHSL(r2, g2, b2);
	const hueDistance = getHueDistance(h1, h2);
	const satDistance = Math.abs(s1 - s2);
	const lightDistance = Math.abs(l1 - l2);
	return (
		hueDistance <= HueThreshold &&
		satDistance <= SatThreshold &&
		lightDistance <= Ligthreshold
	);
}

function checkColor(color1, color2) {
	const { r: r1, g: g1, b: b1 } = color1;
	const { r: r2, g: g2, b: b2 } = color2;
	return checkRGB(r1, g1, b1, r2, g2, b2) || checkHSL(r1, g1, b1, r2, g2, b2);
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
	const [score, setScore] = useState(-1);
	const [showSensor, setShowSensor] = useState(false);
	const [key, setKey] = useState(0);

	function handleCorrectAnswer() {
		setIsCorrect(false);
		setShowSensor(false);
		console.log('correct');
		setScore((prevScore) => prevScore + 1);
		const newColor = genColor();
		setGameColor((prevState) => {
			return { ...prevState, ...newColor };
		});
	}

	function handleSkipAnswer() {
		setIsCorrect(false);
		setShowSensor(false);
		console.log('skip');
		setScore((prevScore) => prevScore - 1);
		const newColor = genColor();
		setGameColor((prevState) => {
			return { ...prevState, ...newColor };
		});
	}

	function update() {
		// var userId = 'XJI27hbv5eVmvEXTaCJTQnhZ33C2';
		var userId = 's1x8XhRgsceMNKnZPV0pvGIVhTb2';
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
		if (checkColor(colorFromUser, gameColor) && key > 1) {
			setIsCorrect(true);
		} else if (score > 0) {
			setShowSensor(true);
		} else if (key > 1 && score === 0) {
			setShowSensor(true);
		}
		if (isOver) {
			setShowSensor(false);
		}
		setKey((prevKey) => prevKey + 1);

		update();
		console.log('---------------');
		console.log('gamecolor : ', gameColor);
		console.log('color from user: ', colorFromUser);
		console.log(timestamp);
		console.log('---------------');
	}, [timestamp]);

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
				<Typography
					sx={{
						color: 'white',
						fontSize: '17px',
						fontStyle: 'italic',
					}}
				>
					Score : {score}
				</Typography>
				<div className="current-color">
					<div className="wrongans">
						<Typography
							color={'white'}
							sx={{
								fontSize: '15px',
								textAlign: 'center',
								marginBottom: '1rem',
								visibility: `${
									!showSensor ? 'hidden' : 'visible'
								}`,
							}}
						>
							Wrong Answer!
						</Typography>
						<Typography
							color={'white'}
							sx={{
								fontSize: '15px',
								textAlign: 'center',
								marginBottom: '1rem',
								visibility: `${
									!showSensor ? 'hidden' : 'visible'
								}`,
							}}
						>
							This is your color
						</Typography>

						<Container
							sx={{
								backgroundColor: `rgb(${colorFromUser.r}, ${colorFromUser.g}, ${colorFromUser.b})`,
								width: 100,
								height: 100,
								borderRadius: '50%',
								visibility: `${
									!showSensor ? 'hidden' : 'visible'
								}`,
							}}
						></Container>
					</div>

					<ColorBox
						color={`rgb(${gameColor.r}, ${gameColor.g}, ${gameColor.b})`}
					/>
					{!isOver ? (
						<CountdownTimer
							isCorrect={isCorrect}
							handleCorrectAnswer={handleCorrectAnswer}
							setIsCorrect={setIsCorrect}
							setIsOver={setIsOver}
							handleSkipAnswer={handleSkipAnswer}
						/>
					) : (
						<div className="timer">
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
											setScore(0);
											setShowSensor(false);
											setKey(2);
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
					FOUND IT?
				</Typography>
				<Typography sx={{ color: 'white', fontSize: '17px' }}>
					{'( PLACE THE ITEM IN FRONT OF THE SENSOR )'}
				</Typography>
				<Typography sx={{ color: 'white', fontSize: '17px' }}>
					• CLAP 3 TIMES
				</Typography>
			</div>
		</div>
	);
}
