import { Typography } from '@mui/material';
import React from 'react';
import CountdownTimer from './CountdownTimer';
import ColorBox from './ColorBox';

export default function Game() {
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
					<CountdownTimer />
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
