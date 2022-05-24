import { Typography } from '@mui/material';
import React, { useState } from 'react';
import { CountdownCircleTimer } from 'react-countdown-circle-timer';

export default function CountdownTimer(props) {
	// const [isPlaying, setIsPlaying] = React.useState(true);
	const [key, setKey] = useState(0);
	return (
		<div className="timer">
			<CountdownCircleTimer
				key={key}
				onComplete={() => {
					console.log('done');
					return { shouldRepeat: true, delay: 2 };
				}}
				onUpdate={(remainingTime) => {
					if (props.isCorrect) {
						setKey((prevKey) => prevKey + 1);
						console.log('update');
						props.handleCorrectAnswer();
					}
					console.log(remainingTime);
				}}
				size={130}
				isPlaying
				duration={60}
				colors="#C18FEE"
			>
				{({ remainingTime, color }) => (
					<Typography sx={{ color, fontSize: 40 }}>
						{remainingTime}
					</Typography>
				)}
			</CountdownCircleTimer>
			{/* <button onClick={() => props.setIsCorrect(true)}>
				Restart Timer
			</button> */}
		</div>
	);
}
