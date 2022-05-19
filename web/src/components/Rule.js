import * as React from 'react';
import Box from '@mui/material/Box';
import Button from '@mui/material/Button';
import Typography from '@mui/material/Typography';
import Modal from '@mui/material/Modal';

const style = {
	position: 'absolute',
	top: '50%',
	left: '50%',
	transform: 'translate(-50%, -50%)',
	width: 400,
	bgcolor: 'background.paper',
	border: '2px solid #000',
	boxShadow: 24,
	p: 4,
	wordWrap: 'break-word',
};

export default function Rule() {
	const [open, setOpen] = React.useState(false);
	const handleOpen = () => setOpen(true);
	const handleClose = () => setOpen(false);

	return (
		<div>
			<Button onClick={handleOpen} sx={{ color: 'white', p: 0 }}>
				Rule
			</Button>
			<Modal
				open={open}
				onClose={handleClose}
				aria-labelledby="modal-modal-title"
				aria-describedby="modal-modal-description"
			>
				<Box sx={style}>
					<Typography
						id="modal-modal-title"
						variant="h6"
						component="h2"
					>
						Rule
					</Typography>
					<Typography id="modal-modal-description" sx={{ mt: 2 }}>
						Duis mollis, est non commodo luctus, nisi erat porttitor
						ligula.sadfjlklkfdsaklsfdklfsdaklfsdakfdskl;fsdllkfsklsfda;klfasdklfadklkflsd;kกดหดกหฟดlfsd;klasfdklfsklfasdklfkdslfklasd
						adfs fsdllkfsklsfdafds fsdllkfsklsfdafdsfads
						fsdllkfsklsfdafdsf
						sadfjlklkfdsaklsfdklfsdaklfsdakfdsklafsd afsd fsd sfd
						adfss dfs dfsa sadfjlklkfdsaklsfdklfsdaklfsdakfdsklafsd
					</Typography>
				</Box>
			</Modal>
		</div>
	);
}
