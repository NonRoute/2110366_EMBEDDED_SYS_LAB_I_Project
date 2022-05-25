import { Typography } from '@mui/material';
import React, { useEffect, useState } from 'react';
import CountdownTimer from './CountdownTimer';
import ColorBox from './ColorBox';
import app from '../utils/firebase';
import { getDatabase, ref, onValue } from 'firebase/database';

export default function Game() {
	const [data, setData] = useState({});
	const [isCorrect, setIsCorrect] = useState(false);
	const [gameColor, setGameColor] = useState('');

	if (data.data === gameColor) {
		setIsCorrect(true);
	}

	function handleCorrectAnswer() {
		console.log('correct');
		//random color
		//props new color
		setIsCorrect(false);
	}

	function update() {
		var userId = 'XJI27hbv5eVmvEXTaCJTQnhZ33C2';
		const dbRef = ref(getDatabase(app));

		onValue(dbRef, function(snapshot) {
			const d = snapshot.val().UsersData[userId].readings;
			setData(d);
		});
		// get(child(dbRef, `UsersData/${userId}`))
		// 	.then((snapshot) => {
		// 		if (snapshot.exists()) {
		// 			const data = snapshot.val()['readings'];
		// 			console.log(data['data']);
		// 			console.log(data['timestamp']);
		// 		} else {
		// 			console.log('No data available');
		// 		}
		// 	})
		// 	.catch((error) => {
		// 		console.error(error);
		// 	});
	}

	useEffect(() => {
		update();
	}, []);

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
					<ColorBox color="red" />
					<CountdownTimer
						isCorrect={isCorrect}
						handleCorrectAnswer={handleCorrectAnswer}
					/>
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
