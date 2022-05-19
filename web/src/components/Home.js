import React from 'react';
import './Home.css';
import Rule from './Rule';
import { Link, Typography } from '@mui/material';

export default function Home() {
	return (
		<div className="main-container">
			<div className="main">
				<Typography
					variant="h3"
					className="rainbow-letters"
					sx={{ marginBottom: '2.5rem' }}
				>
					<span>C</span>
					<span>O</span>
					<span>L</span>
					<span>O</span>
					<span>R</span>
					<span> </span>
					<span>M</span>
					<span>A</span>
					<span>T</span>
					<span>C</span>
					<span>H</span>
					<span>I</span>
					<span>N</span>
					<span>G</span>
				</Typography>
				<Link
					href="/game"
					sx={{
						color: 'white',
						border: 'none',
						textDecoration: 'none',
						marginBottom: '0.5rem',
					}}
				>
					START GAME!
				</Link>
				<Rule />
			</div>
		</div>
	);
}
